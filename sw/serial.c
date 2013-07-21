/**
 * Serial command support.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uart.h"
#include "timer.h"
#include "serial.h"


#define BUFFER_SIZE  20


static char buffer[BUFFER_SIZE];
static unsigned char buffer_length;


/**
 * Called from main() on startup.
 */
void serial_init()
{
  memset(buffer, 0, sizeof(buffer));
  buffer_length = 0;
}

/**
 * Map an ASCII digit 0-3 to one of the timer buffers.
 */
static timer_t *select_time(char num)
{
  timer_t *time = 0;

  switch (num)
  {
    case '0':
      time = &current_time;
      break;

    case '1':
      time = &closing_time;
      break;

    case '2':
      time = &cleanup_time;
      break;

    case '3':
      time = &countdown_time;
      break;
  }

  return time;
}

/**
 * Respond to an ATSTn? command by reporting the current, closing or cleanup time.
 */
static void report_time(const timer_t *time)
{
  if (time->is_set)
  {
    printf("%02d:%02d:%02d\r\n", time->ts.hour, time->ts.minute, time->ts.second);
  }
  else
    printf("Not set\r\n");
}

static void set_time(timer_t *time, const char *buffer, unsigned char buffer_length)
{
  timer_t tmp_time;
  unsigned char match_len;
  char accurate = 0;

  match_len = timer_parse(&tmp_time.ts, buffer, buffer_length);

  if (match_len >= 5)     // hh:mm
  {
    if (time == &current_time)
    {
      if (match_len >= 11)  // hh:mm:ss.cc
        accurate = 1;

      timer_set(&tmp_time.ts, accurate);
    }
    else
    {
      tmp_time.is_set = 1;

      memcpy(time, &tmp_time, sizeof(timer_t));
    }

    printf("OK\r\n");
  }
  else
  {
    printf("ERROR: Time format problem.\r\n");
  }
}

/**
 * Called when \r or \n received, indicating end of a command.
 */
static void process_buffer()
{
  timer_t *time;

  printf("\r\n");

  if (!strncmp(buffer, "at?", 3))
  {
    printf("SpaceTime commands:\r\n");

    printf("  AT? - display this help\r\n");
    printf("  ATSTn? - display time: n=0 - current, 1 - closing, 2 - cleanup, 3 - countdown\r\n");
    printf("  ATSTn=hh:mm:ss.cc set a time - .cc is 100ths of a second, optional\r\n");
    printf("\r\nFor drift correction, set time with .cc twice, at least 1 hr apart.\r\n");
  }
  else
  if (!strncmp(buffer, "atst", 4) && buffer_length >= 6)
  {
    time = select_time(buffer[4]);

    if (time)
    {
      switch (buffer[5])
      {
        case '?':  // ATSTn?
          report_time(time);
          break;

        case '=':  // ATSTn=hh:mm[:ss[.cc]]
          set_time(time, &buffer[6], buffer_length-6);
          break;

        default:
          printf("ERROR: operator '%c' not recognized\r\n", buffer[5]);
          break;
      }
    }
    else
      printf("ERROR: Out of range time selector '%c'\r\n", buffer[4]);
  }
  else
  if (buffer_length == 2 && !strncmp(buffer, "at", 2))
  {
    printf("OK\r\n");
  }
  else
  if (!strncmp(buffer, "atdt", 4))
  {
    printf("BUSY\r\n");
  }
  else
  {
    printf("ERROR: Try AT?\r\n");
  }

  memset(buffer, 0, sizeof(buffer));
  buffer_length = 0;
}

/**
 * Called from main() to check for serial activity.
 */
void serial_update()
{
  char c;

  if (uart_haschar())
  {
    c = uart_getchar();

    if (c == '\r' || c == '\n')
    {
      if (buffer_length > 0)
        process_buffer();
    }
    else
    if (buffer_length < BUFFER_SIZE)
    {
      uart_putchar(c, 0);
      buffer[buffer_length++] = tolower(c);
    }
  }
}

