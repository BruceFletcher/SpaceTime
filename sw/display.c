/**
 * Display functions for SpaceTime project
 */

#include "shift.h"
#include "keypad.h"
#include "display.h"


/**
 * This is set from main() (or somewhere else) to indicate what characters to display.
 */
display_t display[2];

/**
 * This loops 0-7 to drive segments.  In the 0-3 range it also drives the keypad.
 */
static char segment_loop_counter;


/**
 * Called from main() upon system startup.
 */
void display_init()
{
  shift_out(0, 0);
  shift_output_enable(1, 1);

  segment_loop_counter = -1;
}

/**
 * Called from main() repeatedly to drive the display.
 *
 * Segments are scanned in a steady 0-7 loop, and digits are enabled/disabled
 * according to character, intensity, cursor, blink, etc.
 */
void display_update()
{
  unsigned char segment;
  unsigned char digit = 0xf1;

  if (++segment_loop_counter > 7)
    segment_loop_counter = 0;

  segment = (1<<segment_loop_counter);

  shift_out(digit, segment);

  if (segment_loop_counter < 4)
    keypad_scan(segment_loop_counter);
}

