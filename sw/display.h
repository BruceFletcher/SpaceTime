/**
 * Display interface functions and datatypes.
 */


/**
 * # of rows of 7-seg displays on the clock.
 */
#define DISPLAY_ROW_COUNT 2

/**
 * For use with digit_control_t.brightness - varies from 0 (off) to this value.
 */
#define DISPLAY_MAX_BRIGHTNESS  4

/**
 * Types of 'digit' that can be displayed in a single 7-seg character
 */
typedef enum
{
  DISPLAY_CHAR_MIN_DIGIT = 0,
  DISPLAY_CHAR_MAX_DIGIT = 9,
  DISPLAY_CHAR_SPACE,
  DISPLAY_CHAR_UNDERSCORE,
  DISPLAY_CHAR_DASH,
  DISPLAY_CHAR_S,
  DISPLAY_CHAR_H
} display_char_t;

/**
 * Control structure for one character of a 7-segment display.
 */
typedef struct
{
  display_char_t  value;          // The character to be displayed
  char            brightness[2];  // 0-MAX_BRIGHTNESS, 2 values on alternate seconds
  char            underbar;       // Non-zero to add a blinking underbar (i.e. a cursor)
} display_digit_control_t;

/**
 * Control structure for one row of 4 7-seg characters.
 */
typedef struct
{
  display_digit_control_t digit[4];
} display_row_t;

/**
 * Control structure for the full two row display.
 */
extern display_row_t display_buffer[DISPLAY_ROW_COUNT];


/**
 * Initialize display data structures and interrupt handling.
 */
void display_init();

/**
 * Call this frequently from the main loop to drive the display.
 */
void display_update();

