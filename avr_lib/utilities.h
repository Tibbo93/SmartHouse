#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

#define UART_BAUDRATE 19200
#define MAX_SIZE_DEVICE_NAME 20
#define MAX_SIZE_CHANNEL_NAME 10
#define NUM_DIGITAL_CHANNELS 17
#define NUM_FUNS 7
#define MAX_NUM_ARGS 4
#define RX_BUFFER_SIZE 128

#define PIN_LED_0_RED PB6
#define PIN_LED_0_GREEN PB5
#define PIN_LED_0_BLUE PB4

uint8_t get_request(char *buffer);

uint8_t perform_request(char *buffer);

uint8_t get_conf(char **args);

uint8_t get_name(char **args);

uint8_t set_name(char **args);

uint8_t set_channel_name(char **args);

uint8_t get_channel_value(char **args);

uint8_t set_channel_value(char **args);

uint8_t get_temperature(char **args);

void upload_conf(void);

void led_init(void);

#endif