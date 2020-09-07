#include "avr_lib/dht.h"
#include "avr_lib/lcd.h"
#include "avr_lib/printf.h"
#include "avr_lib/uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define UART_BAUDRATE 19200

int main(void) {

    unsigned int c;
    char tmpRxBuffer[16];
    uint8_t idxTmpRxBuffer = 0;

    //initialize uart library, pass baudrate and AVR cpu clock
    uart_init(UART_BAUD_SELECT(UART_BAUDRATE, F_CPU));

    //initialize printf function
    printf_init();

    //initialize display, cursor off
    //lcd_init(LCD_DISP_ON);

    //enable global interrupt flag
    sei();

    DDRB |= (1 << PB4);          //set PB4 as output (led blue)
    DDRB |= (1 << PB1);          //set PB1 as output (led alarm)

    while (1) {
        c = uart_getc();

        if (c & UART_NO_DATA) {
            if (idxTmpRxBuffer != 0) {
                tmpRxBuffer[idxTmpRxBuffer] = '\0';
                idxTmpRxBuffer = 0;

                if (!strcmp(tmpRxBuffer, "on")) {
                    PORTB |= (1 << PB4);
                    PORTB |= (1 << PB1);

                    //lcd_clrscr();
                    //lcd_puts("LED ACCESO");
                }

                if (!strcmp(tmpRxBuffer, "off")) {
                    PORTB &= ~(1 << PB4);
                    PORTB &= ~(1 << PB1);

                    //lcd_clrscr();
                    //lcd_puts("LED SPENTO");
                }

                if (!strcmp(tmpRxBuffer, "temp")) {

                    int8_t temperature = 0, humidity = 0;

                    //initialize temperature and humidity sensor
                    dht_init();
                    if (dht_read(&temperature, &humidity) != -1) {

                        /*char tmp[4];
                        itoa(temperature, tmp, 10);

                        //clear display and home cursor
                        lcd_clrscr();

                        lcd_puts("Temperatura ");
                        lcd_puts(tmp);

                        itoa(humidity, tmp, 10);
                        lcd_puts(" C\nUmidita' ");
                        lcd_puts(tmp);
                        lcd_putc('%');
                    */
                    } else
                        printf("ERROR\n");
                    dht_reset();
                }
            }
        } else {
            if (c & UART_FRAME_ERROR) {
                uart_puts_P("UART Frame Error: ");
            }
            if (c & UART_OVERRUN_ERROR) {
                uart_puts_P("UART Overrun Error: ");
            }
            if (c & UART_BUFFER_OVERFLOW) {
                uart_puts_P("Buffer overflow error: ");
            }

            tmpRxBuffer[idxTmpRxBuffer++] = c;
        }

        _delay_ms(200);
    }
}
