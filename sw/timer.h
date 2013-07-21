/**
 * Time support functions.
 *
 * Most of the complexity in this module surrounds drift correction for current_time.
 */


typedef struct
{
  char hour;
  char minute;
  char second;
  char cent;
} timestamp_t;

typedef struct
{
  timestamp_t ts;
  char is_set;
} timer_t;

typedef enum
{
  NORMAL = 1,
  COUNTDOWN = 2
} timer_mode_t;

extern timer_t current_time;
extern timer_t closing_time;
extern timer_t cleanup_time;
extern timer_t countdown_time;

extern timer_mode_t timer_mode;

/**
 * Initialize timer data structures and interrupt handling.
 *
 * Set *time = null to set up the counter in 'VCR blinking 12:00' mode.
 */
void timer_init(const timestamp_t *time);

/**
 * Set the current time from an external source.
 *
 * If the time seems trustworthy enough for drift calculation, set
 * believed_accurate to a non-zero value.
 */
void timer_set(const timestamp_t *new_time, char believed_accurate);

/**
 * Parse a hh:mm:ss.cc time into *time, returning the # of characters matched.
 */
unsigned char timer_parse(timestamp_t *time, const char *string, char string_len);

