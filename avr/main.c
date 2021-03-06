#include "avr_lib/dht.h"
#include "avr_lib/lcd.h"
#include "avr_lib/requests.h"
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

    char RX_buffer[RX_BUFFER_SIZE] = { 0 };

    //initialize uart library
    uart_init();

    //initialize display and channels
    lcd_init(LCD_DISP_ON);
    digital_out_init();
    digital_in_init();
    adc_init();
    pwm_init();

    //upload configuration
    load_conf();

    lcd_clrscr();

    while (1) {

        memset(RX_buffer, 0, strlen(RX_buffer) + 1);

        uint8_t recvBytes = get_request(RX_buffer);

        if (recvBytes != 0) {
            handle_request(RX_buffer, recvBytes);
        }

        if(is_enable(2))
            detect_fire();

        if(is_enable(1))
            detect_motion();

        _delay_ms(50);
    }
}