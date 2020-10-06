#ifndef UTILITIES_H
#define UTILITIES_H

#define MAX_NUM_ARGS 8
#define MAX_SIZE_DEVICE_NAME 21
#define MAX_SIZE_CHANNEL_NAME 21
#define NUM_DIGITAL_CHANNELS 17
#define READ_BUFFER_SIZE 512

int perform(char buffer[], int fd);

int get_conf(int serial_port);

int get_name(char **args);

int set_name(char **args);

int set_channel_name(char **args);

int get_channel_value(char **args);

int set_channel_value(char **args);

int help();

int query_channel(char **args);

int get_temperature(char **args);

void free_memory();

#endif