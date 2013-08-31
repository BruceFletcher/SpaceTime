/**
 * Watchdog interrupt handler.
 */

/**
 * Use WDTO_xxMS constants from here to chose a timeout value.
 */
#include <avr/wdt.h>


/**
 * Call to initialize the watchdog timer at the start of main()
 */
void watchdog_init(unsigned char wdto_period, void (*timeout_callbark)(void));

/**
 * This must be called regularly to stop the above callback function from being called.
 */
#define watchdog_tick() wdt_reset()

