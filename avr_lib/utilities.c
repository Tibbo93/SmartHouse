#include "utilities.h"
#include "requests.h"
#include "uart.h"
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

uint8_t (*funs[NUM_FUNS])(char **) = {
    get_avr_name,
    get_avr_channels,
    get_name,
    set_name,
    set_channel_name,
    get_channel_value,
    set_channel_value,
    get_temperature,
};

char *funs_name[NUM_FUNS] = {
    "get_avr_name",
    "get_avr_channels",
    "get_name",
    "set_name",
    "set_channel_name",
    "get_channel_value",
    "set_channel_value",
    "get_temperature",
};

struct UART *uart;

void digital_out_init(void) {

    DDRB |= 0xff;
    DDRL |= 0xff;
}

void digital_in_init(void) {

    DDRC = 0;
}

void adc_init(void) {

    //AREF = AVcc
    ADMUX = (1 << REFS0);

    //set prescaler 128 and adc enable
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t ch) {

    //AND operation with 7
    ch &= 0b0000111;

    //clears the bottom 3 bit befor ORing
    ADMUX = (ADMUX & 0xf8) | ch;

    //adc start convertion
    ADCSRA |= (1 << ADSC);

    //wait for conversion complete
    while (ADCSRA & (1 << ADSC))
        ;

    return ADC;
}

void pwm_init(void) {

    TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << CS10);
    TCCR2A = (1 << WGM20) | (1 << COM2A1);
    TCCR2B = (1 << CS20);

    OCR2A = 0;
    OCR1A = 0;
    OCR1B = 0;
}

void uart_init(void) {
    uart = UART_init("uart_0", 19200);
}

void get_request(char *buff) {

    buff[0] = 0;
    int idx = 0;

    while (1) {

        uint8_t c = UART_getChar(uart);
        buff[idx] = c;
        idx++;
        buff[idx] = 0;

        if (c == 0xFF || c == '\n' || c == '\r' || c == 0 )
            break;

        _delay_ms(10);
    }
}

void send_response(char *str) {

    int len = strlen(str);

    for (int i = 0; i < len; i++, str++) {
        UART_putChar(uart, (uint8_t)*str);
    }
}

uint8_t perform_request(char *buffer) {

    char *token;
    char *params[NUM_ARGS] = { 0 };
    int idx = 0;

    token = strtok(buffer, " \n");
    while (token != NULL) {
        params[idx++] = token;
        token = strtok(NULL, " \n");
    }

    if (params[0] != NULL) {
        for (uint8_t i = 0; i < NUM_FUNS; i++) {
            if (strcmp(params[0], funs_name[i]) == 0) {
                return funs[i](params);
            }
        }
    }

    return EXIT_FAILURE;
}
