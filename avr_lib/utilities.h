#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

#define UART_BAUDRATE 19200
#define MAX_SIZE_DEVICE_NAME 21
#define MAX_SIZE_CHANNEL_NAME 21
#define NUM_CHANNELS 32
#define NUM_FUNS 8
#define MAX_NUM_ARGS 4
#define RX_BUFFER_SIZE 128

#define SWITCH_B 0
#define SWITCH_L 1
#define DIGITAL_IN 2
#define ANALOG_IN 3

uint8_t get_request(char *buffer);

uint8_t perform_request(char *buffer);

uint8_t get_avr_name(char **args);

uint8_t get_avr_channels(char **args);

uint8_t get_name(char **args);

uint8_t set_name(char **args);

uint8_t set_channel_name(char **args);

uint8_t get_channel_value(char **args);

uint8_t set_channel_value(char **args);

uint8_t get_temperature(char **args);

void load_conf(void);

void digital_out_init(void);

void digital_in_init(void);

void adc_init(void);

#endif