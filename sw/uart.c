/**
 * UART functions.
 *
 * These functions support buffered, interrupt driven UART input and output.
 * Note the USE_TX_INTERRUPT and DROP_TX_OVERRUNS options carefully.
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"


/**
 * Disable this to use non-interrupt driven transmit.  Receive is always interrupt driven.
 * Non-interrupt driven transmit may blow some significant holes in the realtime performance
 * of your code, but it's left here as an option in case you suspect a bug in the interrupt-
 * driven transmit code.
 */
#if 1
#define USE_TX_INTERRUPT

/**
 * Enable this to drop characters that don't fit the transmit buffer,
 * or disable it to drop the program into slow mode while it waits
 * for a byte to finish reading out, creating buffer space.
 *
 * Enabled gives more stable realtime performance, but disabled
 * makes serial debugging output legible.  The best answer is
 * probably to drop overruns, but ensure debugging messages
 * are short and infrequent.
 */
#if 0
#define DROP_TX_OVERRUNS
#endif
#endif

#define BAUD_PRESCALE (((F_CPU / (UART_BAUDRATE * 16UL))) - 1)

#define UART_RBUF_SIZE                          8   // bytes, must be power of 2, max 256
#define UART_SBUF_SIZE                         64   // bytes, must be power of 2, max 256


char uart_rbuf[UART_RBUF_SIZE];
unsigned char uart_rnext;
unsigned char uart_rlast;

char uart_sbuf[UART_SBUF_SIZE];
volatile unsigned char uart_snext;
unsigned char uart_slast;

int uart_putchar(char c, FILE *stream);

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);


void uart_init()
{
  uart_rnext = 0;
  uart_rlast = 0;

  uart_snext = 0;
  uart_slast = 0;

  // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
  UBRR0H = (unsigned char)(BAUD_PRESCALE >> 8);

  // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
  UBRR0L = (unsigned char)BAUD_PRESCALE;

  UCSR0B |= (1 << RXEN0) | (1 << TXEN0); // Turn on the transmission and reception circuitry
  UCSR0C |= (1 << USBS0) | (3 << UCSZ00); // Use 8-bit character sizes

  UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RX)

  stdout = &uart_str;
}

ISR(USART_RX_vect)
{
  char c = UDR0;
  unsigned char next = (uart_rnext+1)&(UART_RBUF_SIZE-1);

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
    uart_rlast = (uart_rlast+1)&(UART_RBUF_SIZE-1);
  }
  sei();

  return c;
}

int uart_putchar(char c, FILE *stream)
{
#ifdef USE_TX_INTERRUPT
  unsigned char last = (uart_slast+1)&(UART_SBUF_SIZE-1);

#ifndef DROP_TX_OVERRUNS
  while (last == uart_snext) {
    // buffer full, wait...
  }
#else
  if (last != uart_snext)
#endif
  {
    uart_sbuf[uart_slast] = c;
    uart_slast = last;

    UCSR0B |= (1<<UDRIE0);  // ensure the USART Data Ready Interrupt is enabled
  }
#else
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
#endif

  return 0;
}

#ifdef USE_TX_INTERRUPT
/**
 * Stuff one output byte into the USART's output queue.
 *
 * If there are no bytes left to go, turn off the USART Data Ready Interrupt.
 */
ISR(USART_UDRE_vect)
{
  UDR0 = uart_sbuf[uart_snext];

  uart_snext = (uart_snext+1)&(UART_SBUF_SIZE-1);

  if (uart_snext == uart_slast)
    UCSR0B &= ~(1<<UDRIE0);
}
#endif

