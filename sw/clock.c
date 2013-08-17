/**
 * High level behaviours for the clock's display, beep and time management.
 */

#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "display.h"
#include "clock.h"

static void clock_set_brightness(display_row_t *row, char even_brightness, char odd_brightness);
static void clock_prepare_time(display_row_t *display, const timestamp_t *timestamp);

static char is_current_time_frozen;


/**
 * Initialize system state on startup.
 */
void clock_init()
{
  // The display is initialized so current time is blinking 0:00 and
  // closing time is blank.

  display_buffer_initialize(display_buffer, DISPLAY_MAX_BRIGHTNESS);

  // Blink the current time's brightness value to indicate that it isn't set.
  clock_set_brightness(&display_buffer[0], DISPLAY_MAX_BRIGHTNESS, 1);

  // Set the initial time value to 0:00
  clock_prepare_time(&display_buffer[0], &current_time.ts);

  display_buffer[0].separator = DISPLAY_SEPARATOR_BLINK;
  display_buffer[1].separator = DISPLAY_SEPARATOR_OFF;

  display_buffer_clear_row(&display_buffer[1]);

}


void clock_freeze_current_time()
{
  is_current_time_frozen = 1;
}

void clock_unfreeze_current_time()
{
  is_current_time_frozen = 0;
}


/**
 * Set brightness for a display row in even/odd seconds.
 */
static void clock_set_brightness(display_row_t *display, char even_brightness, char odd_brightness)
{
  unsigned char i;

  for (i=0; i<4; ++i)
  {
    display->digit[i].brightness[0] = even_brightness;
    display->digit[i].brightness[1] = odd_brightness;
  }
}


/**
 * Convert a timestamp into a set of 4 digits to display on one clock row.
 *
 * The timestamp is from timer.c, and the output display structure is
 * used by display.c.
 */
static void clock_prepare_time(display_row_t *display, const timestamp_t *timestamp)
{
  display->digit[0].value = (timestamp->hour > 9 ? timestamp->hour / 10 : DISPLAY_CHAR_SPACE);
  display->digit[1].value = timestamp->hour % 10;
  display->digit[2].value = timestamp->minute / 10;
  display->digit[3].value = timestamp->minute % 10;
}


/**
 * Set the current time.
 */
void clock_set_current_time(const timestamp_t *time, char believed_accurate)
{
  // Stop blinking the current time (if we still are) because we have a valid time now.
  clock_set_brightness(&display_buffer[0], DISPLAY_MAX_BRIGHTNESS, DISPLAY_MAX_BRIGHTNESS);

  timer_set(time, believed_accurate);

  printf("Current time: ");
  clock_echo_time(&current_time);
}


/**
 * Set the closing time.
 */
void clock_set_closing_time(const timestamp_t *time)
{
  memcpy(&closing_time.ts, time, sizeof(timestamp_t));
  closing_time.is_set = 1;
  clock_prepare_time(&display_buffer[1], time);
  display_buffer[1].separator = DISPLAY_SEPARATOR_ON;

  printf("Closing time: ");
  clock_echo_time(&closing_time);
}


/**
 * Clear the closing time.
 */
void clock_clear_closing_time()
{
  closing_time.is_set = 0;
  display_buffer_clear_row(&display_buffer[1]);
  display_buffer[1].separator = DISPLAY_SEPARATOR_OFF;

  printf("Closing time: ");
  clock_echo_time(&closing_time);
}


/**
 * Format a time and send it through the serial port.
 */
void clock_echo_time(const timer_t *time)
{
  if (time->is_set)
  {
    printf("%02d:%02d:%02d\r\n", time->ts.hour, time->ts.minute, time->ts.second);
  }
  else
    printf("Not set\r\n");
}


/**
 * Called in the main loop to update clock state.
 */
void clock_update()
{
  static char minute;

  if (minute != current_time.ts.minute && !is_current_time_frozen)
  {
    minute = current_time.ts.minute;
    clock_prepare_time(&display_buffer[0], &current_time.ts);
  }

  if (current_time.is_set && closing_time.is_set)
  {
    if (current_time.ts.hour == closing_time.ts.hour &&
       current_time.ts.minute == closing_time.ts.minute )
    {
      // We've hit closing time!
      // todo: make this more impressive.
      timer_beep(100);
      clock_clear_closing_time();
    }
  }
}

