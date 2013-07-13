/**
 * UART functions.
 *
 * These functions support buffered, interrupt driven UART input, but the output
 * is still a naive block-while-writing implementation.  The output setup should
 * be reimplemented as buffered as well.
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"


#define BAUD_PRESCALE (((F_CPU / (UART_BAUDRATE * 16UL))) - 1)

#define UART_BUF_SIZE                          8   // bytes, must be power of 2

char uart_rbuf[UART_BUF_SIZE];
unsigned char uart_rnext;
unsigned char uart_rlast;


int uart_putchar(char c, FILE *stream) 
{ 
   loop_until_bit_is_set(UCSR0A, UDRE0); 
   UDR0 = c; 
   return 0; 
}    

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void uart_init()
{
  uart_rnext = 0;
  uart_rlast = 0;

  // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
  UBRR0H = (unsigned char)(BAUD_PRESCALE >> 8);

  // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
  UBRR0L = (unsigned char)BAUD_PRESCALE;

  UCSR0B |= (1 << RXEN0) | (1 << TXEN0); // Turn on the transmission and reception circuitry
  UCSR0C |= (1 << USBS0) | (3 << UCSZ00); // Use 8-bit character sizes

#ifdef UART_INTERRUPT_SUPPORT_ENABLED
  UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RX)
#endif

  stdout = &uart_str;
}

ISR(USART_RX_vect)
{
  char c = UDR0;
  unsigned char next = (uart_rnext+1)&(UART_BUF_SIZE-1);

  if (next != uart_rlast)
  {
    uart_rbuf[next] = c;
    uart_rnext = next;
  }
}

inline char uart_haschar()
{
  return (uart_rnext != uart_rlast);
}

char uart_getchar()
{
  char c=0;

  cli();
  if (uart_rnext != uart_rlast)
  {
    c = uart_rbuf[uart_rlast];
    uart_rlast = (uart_rlast+1)&(UART_BUF_SIZE-1);
  }
  sei();

  return c;
}

