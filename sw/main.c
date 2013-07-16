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
  keypad_init();

  printf("\r\n*** BOOTED ***\r\nSpaceTime, yay!\r\n");

  sei();  // enable interrupts

  while(1)
  {
    // Serial echo
    if (uart_haschar())
    {
      c = uart_getchar();

      uart_putchar(c, 0);
    }

    display_update();

    if (keypad.is_valid)
    {
      keypad.is_valid = 0;

      printf("%d\n", (int)keypad.keypress);
    }
  }
}

