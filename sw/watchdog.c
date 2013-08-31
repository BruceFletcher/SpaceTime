/**
 * Watchdog interrupt handler.
 */

#include <avr/interrupt.h>
#include "watchdog.h"


static void (*callback)(void);


/**
 * Call to initialize the watchdog timer at the start of main()
 */
void watchdog_init(unsigned char period, void (*timeout_callbark)(void))
{
  wdt_reset();  // try to avoid getting hit with a watchdog timeout while setting up

  wdt_enable(period);   // set up the timer with a given WDTO_x constant from avr/wdt.h

  WDTCSR |= (1<<WDIE);  // enable the watchdog interrupt

  callback = timeout_callbark;
}

ISR(WDT_vect)
{
  callback();

  WDTCSR |= (1<<WDIE);  // enable the watchdog interrupt (i.e. disable watchdog reset)
}

