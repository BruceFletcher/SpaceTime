/**
 * Keypad interface functions.
 *
 * Relies on display driver to know what row is being scanned out when keypresses are detected.
 */


typedef enum
{
  KEY_MIN_DIGIT = 0,
  KEY_MAX_DIGIT = 9,
  KEY_CLEAR = 10,
  KEY_ENTER,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_CURRENT_TIME,
  KEY_CLOSING_TIME
} keypress_t;

typedef struct
{
  keypress_t keypress;
  char       is_valid;
} keypad_t;

/**
 * This data is debounced and set by this module, but the consumer can
 * clear the is_valid flag once a keypress is consumed.  It will only
 * be set again if a new keypress is detected.
 */
extern volatile keypad_t keypad;

/**
 * Initialize keypad data structures and interrupt handling.
 */
void keypad_init();

/**
 * Called from the display driver to detect keypresses.
 */
void keypad_scan(char row);

