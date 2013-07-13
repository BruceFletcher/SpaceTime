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


int main(void)
{
  char c;

  uart_init();
  shift_init();
  timer_init(0);
  display_init();

  shift_out(0, 0);
  shift_output_enable(1, 1);

  printf("\n*** BOOTED ***\nSpaceTime, yay!\n");

  sei();  // enable interrupts

  while(1)
  {
    if (uart_haschar())
    {
      c = uart_getchar();

      uart_putchar(c, 0);
    }

    if (current_time.ts.second & 1)
      shift_out(0xff, 0xff);
    else
      shift_out(0, 0);
  }
}

