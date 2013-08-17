/**
 * High level behaviours for the clock's display, beep and time management.
 */

/**
 * Initialize system state on startup.
 */
void clock_init();

/**
 * Set the current time.
 */
void clock_set_current_time(const timestamp_t *time, char believed_accurate);

/**
 * Used to stop minute rollover while editing the displayed time.
 */
void clock_freeze_current_time();
void clock_unfreeze_current_time();

/**
 * Set the closing time.
 */
void clock_set_closing_time(const timestamp_t *time);

/**
 * Clear the closing time.
 */
void clock_clear_closing_time();

/**
 * Format a time and send it through the serial port.
 */
void clock_echo_time(const timer_t *time);

/**
 * Called in the main loop to update clock state.
 */
void clock_update();

