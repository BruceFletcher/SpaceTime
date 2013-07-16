/**
 * Display interface functions.
 */


typedef enum
{
  LEVEL_0,
  LEVEL_1,
  LEVEL_2,
  LEVEL_3
} brightness_t;

typedef struct
{
  char digit[4];                // 0-9, [ -_]
  brightness_t brightness[4];
  char blink[4];                // non-zero to blink a digit
  char underbar[4];             // non-zero to underbar a digit
} display_t;

/**
 * Set this from main() to contain whatever you want displayed.
 */
extern display_t display[2];


/**
 * Initialize display data structures and interrupt handling.
 */
void display_init();

/**
 * Call this frequently from the main loop to drive the display.
 */
void display_update();

