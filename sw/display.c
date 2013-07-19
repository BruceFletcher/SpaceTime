/**
 * Display functions for SpaceTime project
 */

#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "shift.h"
#include "keypad.h"
#include "display.h"


/**
 * This is set from main() (or somewhere else) to indicate what characters to display.
 */
display_row_t display_buffer[DISPLAY_ROW_COUNT];

/**
 * This is the copy of the above structure that we're currently scanning out.
 */
static display_row_t current_display[DISPLAY_ROW_COUNT];

/**
 * Bytes that are actually sent out for digit selector value during display updates.
 *
 * Subscripts are:
 *
 * 2 - alternate views updated once per second, using timer.h's current_time.ts.second
 * 4 - A loop of 4 states, for brightness control
 * 8 - Per-segment selector for which digits to enable
 */
static unsigned char output_buffer[2][4][8];

/**
 * Counter for brightness control, should be 0-3, updated after each segment loop
 */
static unsigned char digit_brightness_counter;

/**
 * This loops 0-7 to drive segments.  In the 0-3 range it also drives the keypad.
 */
static unsigned char segment_loop_counter;

/**
 * Segments to enable for each given type of character to be displayed.
 *
 * The order of these character mappings must match enum values in display_char_t.
 */
static const unsigned char character_map[] =
{
  0xfc,  // 0
  0x60,  // 1
  0xda,  // 2
  0xf2,  // 3
  0x66,  // 4
  0xbc,  // 5
  0xbe,  // 6
  0xe0,  // 7
  0xfe,  // 8
  0xe6,  // 9
  0x00,  // space
  0x10,  // underscore
  0x02,  // dash
  0xbc,  // S
  0x6e   // H
};


/**
 * Called from main() upon system startup.
 */
void display_init()
{
  shift_out(0, 0);
  shift_output_enable(1, 1);

  segment_loop_counter = -1;
  digit_brightness_counter = -1;

  memset(display_buffer, 0, sizeof(display_buffer));
  memset(output_buffer, 0, sizeof(output_buffer));
}


/**
 * Called from display_update() whenever a change is made to the display_buffer.
 *
 * This function pre-calculates an array of bytes that are shifted out for the
 * digit selector value on each display_update() call.
 */
static void update_digit_sequence()
{
  unsigned char row, digit, segment, character, d_mask;

  memset(output_buffer, 0, sizeof(output_buffer));
  memcpy(current_display, display_buffer, sizeof(current_display));

  for (row=0; row<DISPLAY_ROW_COUNT; ++row)
  {
    for (digit=0; digit<4; ++digit)
    {
      character = character_map[display_buffer[row].digit[digit].value];
      d_mask = (1 << (row*4+digit));

      for (segment=1; segment<8; ++segment)
      {
        if (character & (1<<segment))
          output_buffer[0][0][segment] |= d_mask;
      }
    }
  }
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
  unsigned char digit;
char updated = 0;

  if (memcmp(display_buffer, current_display, sizeof(display_buffer)))
{
    update_digit_sequence();
updated = 1;
}

  if (++segment_loop_counter > 7)
    segment_loop_counter = 0;

  if (++digit_brightness_counter > 3)
    digit_brightness_counter = 0;

  segment = (1<<segment_loop_counter);

  digit = output_buffer[0][0][segment_loop_counter];
if (updated)
printf("%x\r\n", digit);

  shift_out(digit, segment);

  if (segment_loop_counter < 4)
    keypad_scan(segment_loop_counter);
}

