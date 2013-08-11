/**
 * Keypad functions for SpaceTime project.
 *
 * These functions support scanning of a 4x4 matrix keypad for
 * keypresses.  The design assumes that someone else is driving
 * one of four scan lines high at a time - conveniently, the display
 * driver does that for us.  All the keypad manager has to do
 * is check the four input lines, map keypresses and debounce
 * the output.
 */

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "keypad.h"


/**
 * A mask for the full set of GPIO pins that we scan at once.
 */
#define ROW_MASK ((1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5))

/**
 * A simple state machine for debouncing the keypad.
 */
static enum
{
  SCANNING,
  DEBOUNCING,
  CLEARING
} keypad_state;

/**
 * This buffers the hi/lo state of all 16 keypad buttons,
 * arranged as 1 char per row of raw GPIO pin reads.
 */
static unsigned char row_state[4];

/**
 * A map of row/column numbers to pressed keys.
 *
 * I'd put this in PROGMEM, but it seems to generate spurious results if I do.
 * static const keypress_t keymap[4][4] PROGMEM = {
 */
static const keypress_t keymap[4][4] = {
  { KEY_CLEAR, 0, KEY_ENTER, KEY_CLOSING_TIME },
  { 1, 2, 3, KEY_CURRENT_TIME },
  { 4, 5, 6, KEY_LEFT },
  { 7, 8, 9, KEY_RIGHT }
};

/**
 * And finally, a structure for passing keypad data to the main loop.
 */
volatile keypad_t keypad;


/**
 * Initialize keypad input lines and keypad memory structures.
 *
 * Called once from main() upon startup.
 */
void keypad_init()
{
  DDRC  &= ~((1<<DDC2)|(1<<DDC3)|(1<<DDC4)|(1<<DDC5));         // input pins
  PORTC |= ((1<<PORTC2)|(1<<PORTC3)|(1<<PORTC4)|(1<<PORTC5));  // with pull-ups enabled

  keypad_state = SCANNING;

  memset(&row_state, 0, sizeof(row_state));
  memset((void*)&keypad, 0, sizeof(keypad));
}

/**
 * Check to see if we've detected a single, valid keypress.
 *
 * The keypad is wired as active high with pull-up resistors,
 * which causes the output when a single key is pressed to
 * look like so:
 *
 * 0 1 1 1
 * 1 1 1 1 <- this row
 * 0 1 1 1
 * 0 1 1 1
 * ^
 * this column
 *
 * As a result, the key identification looks a bit messy.
 * Sorry about that.
 *
 * Returns keypress_t value or -1 on absent/invalid input
 */
static keypress_t check_keypress()
{
  unsigned char i, col;
  unsigned char row = 0xff;
  unsigned char col_mask = 0xff;

  for (i=0; i<4; ++i)
  {
    // exactly one row should match the row mask (if a single key is pressed)
    if (row_state[i] == ROW_MASK)
    {
      if (row == 0xff)
        row = i;
      else
        return -1;
    }
    else
    {
      // three other rows should have matching input values
      if (col_mask == 0xff)
        col_mask = row_state[i];
      else
      if (col_mask != row_state[i])
        return -1;
    }
  }

  if ( row == 0xff)
    return -1;  // should never happen

  switch (col_mask)
  {
    case ((1<<PINC3)|(1<<PINC4)|(1<<PINC5)):
      col = 0;
      break;

    case ((1<<PINC2)|(1<<PINC4)|(1<<PINC5)):
      col = 1;
      break;

    case ((1<<PINC2)|(1<<PINC3)|(1<<PINC5)):
      col = 2;
      break;

    case ((1<<PINC2)|(1<<PINC3)|(1<<PINC4)):
      col = 3;
      break;

    default:
      return -1;
  }

  return keymap[row][col];
}

/**
 * Scan the inputs for signs of a keypress.
 *
 * This is called by the display driver as it scans its segment lines.
 */
void keypad_scan(unsigned char row)
{
  static char debounce_count;
  keypress_t key = -1;

  if (row == 0)
  {
    key = check_keypress();

    switch (keypad_state)
    {
      case SCANNING:
        if (key != -1)
        {
          // number of keypad cycles we'll be in the debounce state:
          debounce_count = 5;
          keypad_state = DEBOUNCING;

          keypad.keypress = key;
          keypad.is_valid = 1;
        }
        break;

      case DEBOUNCING:
        if (--debounce_count <= 0)
          keypad_state = CLEARING;
        break;

      case CLEARING:
        if (key == -1)
          keypad_state = SCANNING;
        break;
    }
  }

  row_state[row] = PINC & ROW_MASK;
}

