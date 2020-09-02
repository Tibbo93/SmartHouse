#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "avr_lib/uart.h"
#include "avr_lib/printf.h"

#define UART_BAUDRATE 9600

int main(void)
{

    unsigned int c;
    char tmpRxBuffer[5];
    uint8_t idxTmpRxBuffer = 0;

    //initialize uart library, pass baudrate and AVR cpu clock
    uart_init(UART_BAUD_SELECT(UART_BAUDRATE, F_CPU));

    //initialize printf function
    printf_init();

    //enable global interrupt flag
    sei();

    DDRB |= (1 << 4);

    while (1)
    {
        c = uart_getc();

        if (c & UART_NO_DATA)
        {
            if (idxTmpRxBuffer != 0)
            {
                tmpRxBuffer[idxTmpRxBuffer] = '\0';
                idxTmpRxBuffer = 0;

                if (!strcmp(tmpRxBuffer, "on"))
                    PORTB |= (1 << PB4);

                if (!strcmp(tmpRxBuffer, "off"))
                    PORTB &= ~(1 << PB4);
            }
        }
        else
        {
            if (c & UART_FRAME_ERROR)
            {
                uart_puts_P("UART Frame Error: ");
            }
            if (c & UART_OVERRUN_ERROR)
            {
                uart_puts_P("UART Overrun Error: ");
            }
            if (c & UART_BUFFER_OVERFLOW)
            {
                uart_puts_P("Buffer overflow error: ");
            }

            tmpRxBuffer[idxTmpRxBuffer++] = c;
        }

        _delay_ms(50);
    }
}
