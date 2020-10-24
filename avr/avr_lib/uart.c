#include "uart.h"
#include "buffer_utils.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>

void setBaud19200(void) {
#define BAUD 19200
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
    UCSR0A |= (1 << U2X0);
#endif
#undef BAUD
}

void setBaud38400(void) {
#define BAUD 38400
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
    UCSR0A |= (1 << U2X0);
#endif
#undef BAUD
}

void setBaud57600(void) {
#define BAUD 57600
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
    UCSR0A |= (1 << U2X0);
#endif
#undef BAUD
}

#define UART_BUFFER_SIZE 256

typedef struct UART {
    int tx_buffer[UART_BUFFER_SIZE];
    volatile uint8_t tx_start;
    volatile uint8_t tx_end;
    volatile uint8_t tx_size;

    int rx_buffer[UART_BUFFER_SIZE];
    volatile uint8_t rx_start;
    volatile uint8_t rx_end;
    volatile uint8_t rx_size;

    int baud;
    // hardware uart;
    int uart_num;
} UART;

static UART uart_0;

struct UART *UART_init(const char *device __attribute__((unused)), uint32_t baud) {
    UART *uart = &uart_0;
    uart->uart_num = 0;

    switch (baud) {
    case 19200:
        setBaud19200();
        break;
    case 38400:
        setBaud38400();
        break;
    case 57600:
        setBaud57600();
        break;
    default:
        return 0;
    }

    uart->tx_start = 0;
    uart->tx_end = 0;
    uart->tx_size = 0;
    uart->rx_start = 0;
    uart->rx_end = 0;
    uart->rx_size = 0;

    // 8-bit data
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    // Enable RX and TX */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
    sei();
    return &uart_0;
}

// returns the free space in the buffer
int UART_rxbufferSize(struct UART *uart __attribute__((unused))) {
    return UART_BUFFER_SIZE;
}

// returns the free occupied space in the buffer
int UART_txBufferSize(struct UART *uart __attribute__((unused))) {
    return UART_BUFFER_SIZE;
}

// number of chars in rx buffer
int UART_rxBufferFull(UART *uart) {
    return uart->rx_size;
}

// number of chars in rx buffer
int UART_txBufferFull(UART *uart) {
    return uart->tx_size;
}

// number of free chars in tx buffer
int UART_txBufferFree(UART *uart) {
    return UART_BUFFER_SIZE - uart->tx_size;
}

void UART_putChar(struct UART *uart, uint8_t c) {
    // loops until there is some space in the buffer
    while (uart->tx_size >= UART_BUFFER_SIZE)
        ;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        uart->tx_buffer[uart->tx_end] = c;
        BUFFER_PUT(uart->tx, UART_BUFFER_SIZE);
    }
    // enable transmit interrupt
    UCSR0B |= _BV(UDRIE0);
}

uint8_t UART_getChar(struct UART *uart) {
    //while (uart->rx_size == 0);
    if(uart->rx_size == 0)
        return 0xff;
    uint8_t c;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        c = uart->rx_buffer[uart->rx_start];
        BUFFER_GET(uart->rx, UART_BUFFER_SIZE);
    }
    return c;
}

ISR(USART0_RX_vect) {
    uint8_t c = UDR0;
    if (uart_0.rx_size < UART_BUFFER_SIZE) {
        uart_0.rx_buffer[uart_0.rx_end] = c;
        BUFFER_PUT(uart_0.rx, UART_BUFFER_SIZE);
    }
}

ISR(USART0_UDRE_vect) {
    if (!uart_0.tx_size) {
        UCSR0B &= ~_BV(UDRIE0);
    } else {
        UDR0 = uart_0.tx_buffer[uart_0.tx_start];
        BUFFER_GET(uart_0.tx, UART_BUFFER_SIZE);
    }
}
