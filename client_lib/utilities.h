#ifndef UTILITIES_H
#define UTILITIES_H

#define NUM_FUNS 9
#define MAX_NUM_ARGS 8
#define READ_BUFFER_SIZE 64

int perform(char buffer[], int fd);

int get_name(char **args);

int set_name(char **args);

int get_channel_name(char **args);

int set_channel_name(char **args);

int get_channel_value(char **args);

int set_channel_value(char **args);

int help();

int query_channel(char **args);

int get_temperature(char **args);

#endif