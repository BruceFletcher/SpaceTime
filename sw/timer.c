/**
 * Timer functions for SpaceTime project
 */

#include <string.h>
#include <avr/interrupt.h>
#include "timer.h"

// Using a 14.7456MHz crystal, these timing numbers all work out to nice round integers:
#define TIMER_PRESCALE 256       // each timer tick is this many F_CPU clock cycles
#define TICKS_PER_LOOP 256       // we're using an 8-bit timer
#define TICKS_PER_SECOND (F_CPU/TIMER_PRESCALE)                // way more than an 8-bit timer, so we loop
#define LOOPS_PER_SECOND (TICKS_PER_SECOND/TICKS_PER_LOOP)     // # of timer loops per second at F_CPU/256
                                                               // (225, including a full first loop on setup)


typedef struct
{
  char per_second;
  char per_minute;
  char per_hour;
  char per_day;
} drift_correction_t;


timer_t current_time;
timer_t closing_time;
timer_t cleanup_time;

static drift_correction_t drift_correction;

static __uint24 current_time_minutes_running;
static char current_time_believed_accurate;

static unsigned char timer_loop_count;


void timer_init(const timestamp_t *time)
{
  unsigned char initial_ticks;
  unsigned char initial_loops;
  int tmp;

  TIMSK0 &= ~(1 << TOIE0);  // Disable timer overflow interrupt
  TCCR0B |= (1 << CS02);    // Configure timer for F_CPU / 256

  if (time == 0)
  {
    memset(&current_time, 0, sizeof(current_time));
  }
  else
  {
    memcpy(&current_time.ts, time, sizeof(timestamp_t));
    current_time.is_set = 1;
  }

  if (current_time.ts.cent == 0)
  {
    initial_ticks = TICKS_PER_LOOP-1;
    initial_loops = LOOPS_PER_SECOND;
  } else {
    tmp = (int)current_time.ts.cent * LOOPS_PER_SECOND;

    initial_loops = tmp / 100;

    tmp -= (int)initial_loops * 100;

    if (tmp == 0)
    {
      initial_loops--;
      initial_ticks = TICKS_PER_LOOP - 1;
    } else {
      initial_ticks = tmp;
    }
  }

  TCNT0 = initial_ticks;             // Initial overflow value (one full loop, basically)
  timer_loop_count = initial_loops;  // Including 'remainder' loop above

  TIMSK0 |= (1 << TOIE0);  // Enable timer overflow interrupt

  closing_time.is_set = 0;
  cleanup_time.is_set = 0;
  current_time_minutes_running = 0;
}


ISR(TIMER0_OVF_vect)
{
  int offset = 0;
  int lo_byte = 0;
  char offset_hi;
  char offset_lo;

  if (--timer_loop_count == 0)
  {
    timer_loop_count = LOOPS_PER_SECOND;

    offset = drift_correction.per_second;

    if (++current_time.ts.second >= 60)
    {
      current_time.ts.second = 0;
      current_time_minutes_running++;

      offset += drift_correction.per_minute;

      if (++current_time.ts.minute >= 60)
      {
        current_time.ts.minute = 0;

        offset += drift_correction.per_hour;

        if (++current_time.ts.hour >= 24)
        {
          current_time.ts.hour = 0;

          offset += drift_correction.per_day;
        }
      }
    }

    if (offset < 0)
    {
      offset = -offset;

      offset_hi = offset >> 8;
      offset_lo = offset & 0xff;

      timer_loop_count -= offset_hi;

      if (offset_lo)
      {
        lo_byte = TCNT0 - offset_lo;

        if (lo_byte > 0)
          TCNT0 = lo_byte;
      }
    }
    else
    if (offset > 0)
    {
      offset_hi = offset >> 8;
      offset_lo = offset & 0xff;

      timer_loop_count += offset_hi;

      lo_byte = TCNT0 + offset_lo;

      if (lo_byte > 255)
      {
        TCNT0 += lo_byte & 0xff;
        ++timer_loop_count;
      }
    }
  }
}

/**
 * Return -1, 0 or 1 if 'this' is less than, equal to or greater than 'that'.
 *
 * This function ignores the 'day' term in timestamp_t, since that is a
 * construct for tracking runtimes not for time/date comparisons.
 *
 * This means that dates are not handled, and so this function presumes
 * that times are within 12 hours of each other - i.e. 23:00 is before 1:00.
 */
static char timer_compare(const timestamp_t *this, const timestamp_t *that)
{
  char diff;

  diff = this->hour - that->hour;
  if (diff > 0)
  {
    if (diff <= 12)
      return 1;
    else
      return -1;
  }
  else
  if (diff < 0)
  {
    if (diff >= -12)
      return -1;
    else
      return 1;
  }

  diff = this->minute - that->minute;
  if (diff > 0)
    return 1;
  else
  if (diff < 0)
    return -1;

  diff = this->second - that->second;
  if (diff > 0)
    return 1;
  else
  if (diff < 0)
    return -1;

  diff = this->cent - that->cent;
  if (diff > 0)
    return 1;
  else
  if (diff < 0)
    return -1;

  return 0;
}

static void timer_subtract(const timestamp_t *this, timestamp_t *from_this)
{
}

void timer_set(const timestamp_t *new_time, char believed_accurate)
{
  timestamp_t old_time;
  timestamp_t diff_time;
  unsigned int loop_count;
  // char full_reset = 0;
  char comparison;
  // int temp;

  if (!believed_accurate || !current_time_believed_accurate)
  {
    timer_init(new_time);
    current_time_believed_accurate = believed_accurate;
  }
  else
  {
    cli();
    memcpy(&old_time, &current_time.ts, sizeof(timestamp_t));
    loop_count = timer_loop_count;
    sei();

    old_time.cent = (loop_count * 100) / LOOPS_PER_SECOND;

    memcpy(&diff_time, new_time, sizeof(timestamp_t));

    comparison = timer_compare(&old_time, &diff_time);

    if (comparison > 0)
    {
      timer_subtract(&old_time, &diff_time);
    }
  }
}

