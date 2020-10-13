#include "avr_lib/dht.h"
#include "avr_lib/lcd.h"
#include "avr_lib/printf.h"
#include "avr_lib/uart.h"
#include "avr_lib/utilities.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

int main(void) {

    uint8_t req;
    char rxBuffer[RX_BUFFER_SIZE];

    //initialize uart library, pass baudrate and AVR cpu clock
    uart_init(UART_BAUD_SELECT(UART_BAUDRATE, F_CPU));

    //enable global interrupt flag
    sei();

    //initialize printf function
    printf_init();

    //initialize display, RGB led and sensors
    //lcd_init(LCD_DISP_ON);
    led_init();

    //upload configuration
    load_conf();

    DDRB |= (1 << PB1);           //set PB1 as output (led alarm - motion)
    DDRB &= ~(1 << PB0);          //set PB1 as input (motion sensor)
    DDRB |= (1 << PB3);           //set PB3 as output (alarm buzzer - motion)

    //DDRG |= (1 << PG5);           //set PG5 as output (led alarm - fire) CAMBIATO
    DDRH &= ~(1 << PH4);          //set PH4 as input (fire sensor)
    DDRH |= (1 << PH5);           //set PH5 as output (alarm buzzer - fire)

    memset(rxBuffer, 0, sizeof(rxBuffer));

    while (1) {

        req = get_request(rxBuffer);

        if (req == 1) {
            perform_request(rxBuffer);
        }

        memset(rxBuffer, 0, sizeof(rxBuffer));

        /*motion detected
        if (((PINB) & (1 << PB0)) == 1) {
            PORTB |= (1 << PB1);          ///turn on alarm led - motion
            lcd_clrscr();
            lcd_puts("MOVIMENTO\n RILEVATO\n");
            for (int8_t i = 0; i < 4; i++) {
                PORTB |= (1 << PB3);
                _delay_ms(200);
                PORTB &= ~(1 << PB3);          //turn of buzzer alarm - motion
                _delay_ms(200);
            }
            lcd_clrscr();
        }

        //motion not detected
        else {
            PORTB &= ~(1 << PB1);          //turn off alarm led - motion
        }

        //fire detected
        if (((PINH) & (1 << PH4)) == 0) {
            PORTG |= (1 << PG5);          //turn on alarm led - fire
            lcd_clrscr();
            lcd_puts("ALLARME INCENDIO");
            for (int8_t j = 0; j < 4; j++) {
                PORTH |= (1 << PH5);
                _delay_ms(200);
                PORTH &= ~(1 << PH5);          //turn of buzzer alarm - fire
                _delay_ms(200);
            }
            lcd_clrscr();
        }

        //fire not detected
        else {
            PORTG &= ~(1 << PG5);          //turn off alarm led - fire
        }

        if (c & UART_NO_DATA) {
            if (idxTmpRxBuffer != 0) {
                tmpRxBuffer[idxTmpRxBuffer] = '\0';

                if (!strcmp(tmpRxBuffer, "on")) {
                    PORTB |= (1 << PB4);

                    lcd_clrscr();
                    lcd_puts("LED ACCESO");
                }

                else if (!strcmp(tmpRxBuffer, "off")) {
                    PORTB &= ~(1 << PB4);

                    lcd_clrscr();
                    lcd_puts("LED SPENTO");
                }
            }
        }*/
    }
}