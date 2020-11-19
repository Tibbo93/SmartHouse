#ifndef REQUESTS_H
#define REQUESTS_H

#include <stdint.h>

void load_conf(void);

uint8_t get_avr_name(char **args, char *payload);

uint8_t get_avr_channels(char **args, char *payload);

uint8_t set_name(char **args, char *payload);

uint8_t set_channel_name(char **args, char *payload);

uint8_t get_channel_value(char **args, char *payload);

uint8_t set_channel_value(char **args, char *payload);

uint8_t get_temperature(char **args, char *payload);

uint8_t is_enable(uint8_t ch);

#endif