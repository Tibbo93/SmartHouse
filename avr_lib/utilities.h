#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

#define UART_BAUDRATE 19200
#define MAX_SIZE_NAME 32
#define NUM_FUNS 8
#define MAX_NUM_ARGS 4
#define RX_BUFFER_SIZE 128

uint8_t get_request(char *buffer);

uint8_t perform_request(char *buffer);

uint8_t get_name(char **args);

uint8_t set_name(char **args);

uint8_t get_channel_name(char **args);

uint8_t set_channel_name(char **args);

uint8_t get_channel_value(char **args);

uint8_t set_channel_value(char **args);

uint8_t query_channel(char **args);

uint8_t get_temperature(char **args);

#endif