/**
 * SpaceTime - A two-row 7-segment LED clock for tracking open hours at VHS.
 *
 * By Bruce Fletcher
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "shift.h"
#include "timer.h"
#include "display.h"
#include "keypad.h"


void prepare_time(display_row_t *display, const timestamp_t *timestamp)
{
if (1 || (current_time.ts.second & 1))
{
  display->digit[0].value = (timestamp->hour > 9 ? timestamp->hour / 10 : DISPLAY_CHAR_SPACE);
  display->digit[1].value = timestamp->hour % 10;
  display->digit[2].value = timestamp->minute / 10;
  display->digit[3].value = timestamp->minute % 10;
} else {
  display->digit[0].value = DISPLAY_CHAR_SPACE;
  display->digit[1].value = DISPLAY_CHAR_SPACE;
  display->digit[2].value = DISPLAY_CHAR_SPACE;
  display->digit[3].value = DISPLAY_CHAR_SPACE;
}
}

int main(void)
{
  char c;

  uart_init();
  shift_init();
  timer_init(0);
  display_init();
  keypad_init();

  printf("\r\n*** BOOTED ***\r\nSpaceTime, yay!\r\n");

  prepare_time(&display_buffer[1], &closing_time.ts);

  sei();  // enable interrupts

  while(1)
  {
    // Serial echo
    if (uart_haschar())
    {
      c = uart_getchar();

      uart_putchar(c, 0);
    }

    prepare_time(&display_buffer[0], &current_time.ts);

    display_update();

    if (keypad.is_valid)
    {
      keypad.is_valid = 0;

      printf("%d\r\n", keypad.keypress);
    }
  }
}

