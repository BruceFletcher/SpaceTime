/**
 * Shift register support.
 */

#include <avr/io.h>

#include "shift.h"


void shift_init()
{
  PORTB |= (1<<PORTB1)   // OE_BAR hi, disable second byte output
        |  (1<<PORTB2);  // OE_BAR hi, disable first  byte output

  DDRB |= (1<<DDB1)      // output enable, second byte (active lo)
       |  (1<<DDB2)      // output enable, first byte (active lo)
       |  (1<<DDB4);     // shifted data

  DDRC |= (1<<DDC0)      // STCP - storage clock
       |  (1<<DDC1);     // SHCP - shift clock
}

void shift_byte(unsigned char c)
{
  char i;

  for (i=0; i<8; ++i)
  {
    if (c&1)
      PORTB |= (1<<PORTB4);  // serial data hi
    else
      PORTB &= ~(1<<PORTB4); // serial data lo

    PORTC &= ~(1<<PORTC1);  // SHCP lo
    PORTC |=  (1<<PORTC1);  // SHCP hi

    c >>= 1;
  }
}

void shift_out(unsigned char digit_select, unsigned char segment_select)
{
  shift_byte(digit_select);
  shift_byte(segment_select);

  PORTC &= ~(1<<PORTC0);  // STCP lo
  PORTC |=  (1<<PORTC0);  // STCP hi
}

void shift_output_enable(char digit_enable, char segment_enable)
{
  if (digit_enable)
    PORTB &= ~(1<<PORTB2);    // OE_BAR lo
  else
    PORTB |= (1<<PORTB2);     // OE_BAR hi

  if (segment_enable)
    PORTB &= ~(1<<PORTB1);    // OE_BAR lo
  else
    PORTB |= (1<<PORTB1);     // OE_BAR hi
}

void shift_disable(void)
{
  PORTB |= (1<<PORTB1) | (1<<PORTB2); // shut 'em down right quick
}

