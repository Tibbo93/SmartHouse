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

char EEMEM eepromName[MAX_SIZE_DEVICE_NAME];
uint8_t EEMEM eepromNameSet;
channel EEMEM eepromDigitalChannels[NUM_DIGITAL_CHANNELS];

char name[MAX_SIZE_DEVICE_NAME] = { 0 };
uint8_t name_set = 0;
channel digitalChannels[NUM_DIGITAL_CHANNELS];

uint8_t (*funs[NUM_FUNS])(char **) = {
    get_conf,
    get_name,
    set_name,
    set_channel_name,
    get_channel_value,
    set_channel_value,
    get_temperature,
};

char *funs_name[NUM_FUNS] = {
    "get_conf",
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
                return 1;
            } else {
                break;
            }
        } else {

            if (c & UART_FRAME_ERROR) {
                uart_puts_P("UART Frame Error: ");
                return -1;
            }

            if (c & UART_OVERRUN_ERROR) {
                uart_puts_P("UART Overrun Error: ");
                return -1;
            }

            if (c & UART_BUFFER_OVERFLOW) {
                uart_puts_P("Buffer overflow error: ");
                return -1;
            }

            buffer[idxRxBuffer++] = c;
        }
        _delay_ms(50);
    }

    return 0;
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

    return -1;
}

uint8_t get_conf(char **args) {

    if (name_set == 255 || name_set == 0)
        uart_putc('0');
    else
        uart_puts(name);
    uart_putc('\n');

    for (uint8_t i = 0; i < NUM_DIGITAL_CHANNELS; i++) {
        if (digitalChannels[i].nameSet == 255 || digitalChannels[i].nameSet == 0) {
            uart_putc('0');
            uart_putc('\n');
        } else {
            uart_puts(digitalChannels[i].name);
            uart_putc('\n');
        }
    }

    return 1;
}

uint8_t get_name(char **args) {

    if (args[1] != NULL)
        return -1;

    name_set = eeprom_read_byte(&eepromNameSet);
    _delay_ms(20);

    if (name_set == 255 || name_set == 0)
        uart_putc('0');
    else {
        eeprom_read_block(name, eepromName, sizeof(eepromName));
        _delay_ms(20);
        uart_puts(name);
    }

    return 1;
}

uint8_t set_name(char **args) {

    if (args[1] == NULL)
        return -1;

    eeprom_update_block(args[1], eepromName, sizeof(eepromName));
    _delay_ms(20);

    eeprom_update_byte(&eepromNameSet, 1);
    _delay_ms(20);

    name_set = 1;
    strcpy(name, args[1]);

    return 1;
}

uint8_t set_channel_name(char **args) {

    uint8_t idx = atoi(args[1]);

    eeprom_update_block(args[2], eepromDigitalChannels[idx].name, sizeof(eepromDigitalChannels[idx].name));
    _delay_ms(20);

    eeprom_update_byte(&eepromDigitalChannels[idx].nameSet, 1);
    _delay_ms(20);

    digitalChannels[idx].nameSet = 1;
    strcpy(digitalChannels[idx].name, args[2]);

    return 1;
}

uint8_t get_channel_value(char **args) {
    return 0;
}

uint8_t set_channel_value(char **args) {
    return 0;
}

uint8_t get_temperature(char **args) {

    if (args[1] != NULL)
        return -1;

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
        return -1;
    }

    dht_reset();
    return 1;
}

void led_init(void) {

    DDRB = (1 << PIN_LED_0_RED);            //set PIN 12 as output
    DDRB = (1 << PIN_LED_0_GREEN);          //set PIN 11 as output
    DDRB = (1 << PIN_LED_0_BLUE);           //set PIN 10 as output
}

void upload_conf(void) {

    name_set = eeprom_read_byte(&eepromNameSet);
    _delay_ms(20);

    eeprom_read_block(name, eepromName, sizeof(eepromName));
    _delay_ms(20);

    eeprom_read_block(digitalChannels, eepromDigitalChannels, sizeof(eepromDigitalChannels));
    _delay_ms(50);
}
