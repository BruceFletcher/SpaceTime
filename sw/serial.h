/**
 * Serial command support.
 *
 * This module implements a sort of 'AT' command set for
 * serial communication between this clock and some sort of
 * host computer.
 */

// Called from main() at the start of the run.
void serial_init();

// Called from main() repeatedly to scan for serial input and process commands.
void serial_update();

