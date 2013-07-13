/**
 * Keypad functions for SpaceTime project
 */

#include <avr/io.h>
#include "keypad.h"


volatile keypad_t keypad;


void keypad_init()
{
  DDRC  &= ~((1<<DDC2)|(1<<DDC3)|(1<<DDC4)|(1<<DDC5));         // input pins
  PORTC |= ((1<<PORTC2)|(1<<PORTC3)|(1<<PORTC4)|(1<<PORTC5));  // with pull-ups enabled
}

void keypad_scan(char row)
{
}

