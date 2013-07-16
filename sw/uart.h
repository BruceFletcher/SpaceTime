/**
 * UART (USART) support for stdout functions including printf()
 */

#define UART_BAUDRATE      115200

/**
 * Call to initialize the UART at the start of main()
 */
void uart_init();

/**
 * Used primarily by stdout functions, but can be called directly.
 *
 * Set stream to 0; it's just there to match stdout expectations.
 */
int uart_putchar(char c, FILE *stream);

/**
 * Functions for reading characters out of the receive buffer.
 *
 * You should only call uart_getchar() if you've received a
 * non-zero return from uart_haschar().
 */
char uart_haschar();
char uart_getchar();

