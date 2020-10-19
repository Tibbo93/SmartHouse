#include "requests.h"
#include "dht.h"
#include "lcd.h"
#include "uart.h"
#include "utilities.h"
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

char EEMEM name_ee[MAX_SIZE_DEVICE_NAME];
uint8_t EEMEM name_set_ee;
Channel EEMEM channels_ee[NUM_CHANNELS];

char name_P[MAX_SIZE_DEVICE_NAME] = { 0 };
uint8_t name_set_P = 0;
Channel channels_P[NUM_CHANNELS] = { 0 };

void load_conf(void) {

    name_set_P = eeprom_read_byte(&name_set_ee);
    _delay_ms(50);

    eeprom_read_block(&name_P, &name_ee, sizeof(name_ee));
    _delay_ms(50);

    eeprom_read_block(&channels_P, &channels_ee, sizeof(channels_ee));
    _delay_ms(50);
}

uint8_t get_avr_name(char **args) {

    if (name_set_P == 255 || name_set_P == 0)
        send_response("0");
    else {
        send_response(name_P);
    }

    return EXIT_SUCCESS;
}

uint8_t get_avr_channels(char **args) {

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        if (channels_P[i].nameSet == 255 || channels_P[i].nameSet == 0)
            send_response("0");
        else
            send_response(channels_P[i].name);
        send_response("\n");
    }

    return EXIT_SUCCESS;
}

uint8_t get_name(char **args) {

    if (args[1] != NULL)
        return -1;

    name_set_P = eeprom_read_byte(&name_set_ee);
    _delay_ms(20);

    if (name_set_P == 255 || name_set_P == 0)
        send_response("0");
    else {
        eeprom_read_block(&name_P, &name_ee, sizeof(name_ee));
        _delay_ms(20);
        send_response(name_P);
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
    char str[2] = { 0 };

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

    sprintf(str, "%d", val);
    send_response(str);
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
        if (bit == 4)
            OCR2A = val;
        else if (bit == 5)
            OCR1A = val;
        else if (bit == 6)
            OCR1B = val;
        else {
            if (val == 1)
                PORTB |= (1 << bit);
            else
                PORTB &= ~(1 << (bit));
        }
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

        char tmp[4] = { 0 };
        itoa(temperature, tmp, 10);
        send_response(tmp);
        send_response(" ");

        //clear display and home cursor
        //lcd_clrscr();

        //lcd_puts("Temperatura ");
        //lcd_puts(tmp);

        itoa(humidity, tmp, 10);
        send_response(tmp);

        //lcd_puts(" C\nUmidita' ");
        //lcd_puts(tmp);
        //lcd_putc('%');

    } else {
        return EXIT_FAILURE;
    }

    dht_reset();
    return EXIT_SUCCESS;
}
