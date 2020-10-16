#include "utilities.h"
#include "dht.h"
#include "lcd.h"
#include "uart.h"
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

typedef struct {
    uint8_t nameSet;
    char name[MAX_SIZE_CHANNEL_NAME];
} channel;

char EEMEM name_ee[MAX_SIZE_DEVICE_NAME];
uint8_t EEMEM name_set_ee;
channel EEMEM channels_ee[NUM_CHANNELS];

char name_P[MAX_SIZE_DEVICE_NAME] = { 0 };
uint8_t name_set_P = 0;
channel channels_P[NUM_CHANNELS];

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

uint8_t get_request(char *buffer) {

    unsigned int c;
    uint8_t idxRxBuffer = 0;

    while (1) {

        c = uart_getc();

        if (c & UART_NO_DATA) {
            if (idxRxBuffer != 0) {
                buffer[idxRxBuffer] = '\0';
                idxRxBuffer = 0;
                return EXIT_SUCCESS;
            } else {
                break;
            }
        } else {
            if (c & UART_FRAME_ERROR) {
                uart_puts_P("UART Frame Error: ");
                return EXIT_FAILURE;
            }
            if (c & UART_OVERRUN_ERROR) {
                uart_puts_P("UART Overrun Error: ");
                return EXIT_FAILURE;
            }
            if (c & UART_BUFFER_OVERFLOW) {
                uart_puts_P("Buffer overflow error: ");
                return EXIT_FAILURE;
            }

            buffer[idxRxBuffer++] = c;
        }
        _delay_ms(20);
    }

    return EXIT_FAILURE;
}

uint8_t perform_request(char *buffer) {

    char *token;
    char *params[MAX_NUM_ARGS] = { 0 };
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

uint8_t get_avr_name(char **args) {

    if (name_set_P == 255 || name_set_P == 0)
        uart_putc('0');
    else {
        uart_puts(name_P);
    }

    return EXIT_SUCCESS;
}

uint8_t get_avr_channels(char **args) {

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        if (channels_P[i].nameSet == 255 || channels_P[i].nameSet == 0)
            uart_putc('0');
        else
            uart_puts(channels_P[i].name);
        uart_putc('\n');
    }

    return EXIT_SUCCESS;
}

uint8_t get_name(char **args) {

    if (args[1] != NULL)
        return -1;

    name_set_P = eeprom_read_byte(&name_set_ee);
    _delay_ms(20);

    if (name_set_P == 255 || name_set_P == 0)
        uart_putc('0');
    else {
        eeprom_read_block(&name_P, &name_ee, sizeof(name_ee));
        _delay_ms(20);
        uart_puts(name_P);
    }

    return EXIT_SUCCESS;
}

uint8_t set_name(char **args) {

    if (args[1] == NULL)
        return EXIT_FAILURE;

    eeprom_update_block(args[1], &name_ee, sizeof(name_ee));
    _delay_ms(50);

    eeprom_update_byte(&name_set_ee, 1);
    _delay_ms(50);

    name_set_P = 1;
    strcpy(name_P, args[1]);

    return EXIT_SUCCESS;
}

uint8_t set_channel_name(char **args) {

    uint8_t idx = atoi(args[1]);

    eeprom_update_block(args[2], &channels_ee[idx].name, sizeof(channels_ee[idx].name));
    _delay_ms(100);

    eeprom_update_byte(&channels_ee[idx].nameSet, 1);
    _delay_ms(100);

    channels_P[idx].nameSet = 1;
    strcpy(channels_P[idx].name, args[2]);

    return EXIT_SUCCESS;
}

uint8_t get_channel_value(char **args) {

    uint8_t ch, type, bit;
    uint16_t val = 0;

    ch = atoi(args[1]);
    type = ch / 8;
    bit = ch % 8;

    switch (type) {
    case SWITCH_B:
        val = ((PORTB & (1 << bit)) != 0);
        break;
    case SWITCH_L:
        val = ((PORTL & (1 << bit)) != 0);
        break;
    case DIGITAL_IN:
        val = ((PORTC & (1 << bit)) != 0);
        break;
    case ANALOG_IN:
        val = adc_read(ch);
        break;
    default:
        break;
    }

    uart_putc(val + '0');
    return EXIT_SUCCESS;
}

uint8_t set_channel_value(char **args) {

    uint8_t type, val, bit, ch;

    val = atoi(args[2]);
    ch = atoi(args[1]);
    type = ch / 8;
    bit = ch % 8;

    switch (type) {
    case SWITCH_B:
        if (val == 1)
            PORTB |= (1 << bit);
        else
            PORTB &= ~(1 << (bit));
        break;
    case SWITCH_L:
        if (val == 1)
            PORTL |= (1 << bit);
        else
            PORTL &= ~(1 << (bit));
        break;
    default:
        break;
    }

    return EXIT_SUCCESS;
}

uint8_t get_temperature(char **args) {

    if (args[1] != NULL)
        return EXIT_FAILURE;

    int8_t temperature = 0, humidity = 0;

    //initialize temperature and humidity sensor
    dht_init();
    if (dht_read(&temperature, &humidity) != -1) {

        char tmp[4];
        itoa(temperature, tmp, 10);
        uart_puts(tmp);
        uart_putc(' ');

        //clear display and home cursor
        lcd_clrscr();

        lcd_puts("Temperatura ");
        lcd_puts(tmp);

        itoa(humidity, tmp, 10);
        uart_puts(tmp);

        lcd_puts(" C\nUmidita' ");
        lcd_puts(tmp);
        lcd_putc('%');

    } else {
        return EXIT_FAILURE;
    }

    dht_reset();
    return EXIT_SUCCESS;
}

void load_conf(void) {

    name_set_P = eeprom_read_byte(&name_set_ee);
    _delay_ms(50);

    eeprom_read_block(&name_P, &name_ee, sizeof(name_ee));
    _delay_ms(50);

    eeprom_read_block(&channels_P, &channels_ee, sizeof(channels_ee));
    _delay_ms(50);
}

void digital_out_init(void) {

    DDRB |= 0xff;
    DDRL |= 0xff;
}

void digital_in_init(void) {

    DDRC = 0x00;
}

void adc_init(void) {

    ADMUX = (1 << REFS0);                                                       //AREF = AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);          //set prescaler 128 and adc enable
}

uint16_t adc_read(uint8_t ch) {

    ch &= 0b0000111;                      //AND operation with 7
    ADMUX = (ADMUX & 0xf8) | ch;          //clears the bottom 3 bit befor ORing

    ADCSRA |= (1 << ADSC);          //adc start convertion

    while (ADCSRA & (1 << ADSC))
        ;          //wait for conversion complete

    return ADC;
}