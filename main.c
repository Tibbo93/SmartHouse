#include "avr_lib/dht.h"
#include "avr_lib/lcd.h"
#include "avr_lib/uart.h"
#include "avr_lib/utilities.h"
#include "avr_lib/requests.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

int main(void) {

    char rx_buffer[RX_BUFFER_SIZE];

    //initialize uart library, pass baudrate and AVR cpu clock
    uart_init();

    //initialize display and channels
    //lcd_init(LCD_DISP_ON);
    digital_out_init();
    digital_in_init();
    adc_init();
    pwm_init();

    //upload configuration
    load_conf();

    while (1) {

        get_request(rx_buffer);

        //send_response(rx_buffer);

        perform_request(rx_buffer);

        /*memset(rx_buffer, 0, sizeof(rx_buffer));

        motion detected
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