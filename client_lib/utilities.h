#ifndef UTILITIES_H
#define UTILITIES_H

#define SWITCH_B 0
#define SWITCH_L 1
#define DIGITAL_IN 2
#define ANALOG_IN 3

#define MAX_NUM_ARGS 8
#define MAX_SIZE_DEVICE_NAME 21
#define MAX_SIZE_CHANNEL_NAME 21
#define READ_BUFFER_SIZE 512

int get_channel(char *name);

int copy_channel(char *token, char **channels, char **def, int idx);

int perform(char buffer[], int fd);

int download_conf(int fd);

int get_avr_name_conf(void);

int get_avr_channels_conf(void);

int get_name(char **args);

int set_name(char **args);

int set_channel_name(char **args);

int get_channel_value(char **args);

int set_channel_value(char **args);

int help();

int query_channel(char **args);

int get_temperature(char **args);

void free_memory(void);

#endif