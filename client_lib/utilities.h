#ifndef UTILITIES_H
#define UTILITIES_H

#define NUM_FUNS 9
#define MAX_NUM_ARGS 8
#define NUM_DIGITAL_CHANNELS 17
#define MAX_SIZE_DEVICE_NAME 20 
#define NUM_DIGITAL_CHANNELS 17
#define READ_BUFFER_SIZE 512

int perform(char buffer[], int fd);

int get_conf(char **args);

int get_name(char **args);

int set_name(char **args);

int set_channel_name(char **args);

int get_channel_value(char **args);

int set_channel_value(char **args);

int help();

int query_channel(char **args);

int get_temperature(char **args);

#endif