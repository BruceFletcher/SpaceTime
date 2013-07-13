/**
 * UART (USART) support for stdout functions including printf()
 */

#define UART_INTERRUPT_SUPPORT_ENABLED
#define UART_BAUDRATE                     115200

void uart_init();
int uart_putchar(char c, FILE *stream);
char uart_haschar();
char uart_getchar();

