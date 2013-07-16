/**
 * Shift register support.
 *
 * Designed to work with two 74HC595 shift registers and to
 * take 16bit words to shift out at a time.
 */

// initialize pins required for the shift register interface.
void shift_init();

// take two bytes and shift them out - low bit to Q7, high bit to Q0.
void shift_out(unsigned char digit_select, unsigned char segment_select);

// 0 tri-states a shift register, 1 enables output.
void shift_output_enable(char digit_output, char segment_output);

