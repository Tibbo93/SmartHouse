#ifndef REQUEST_H
#define REQUEST_H

#define READ_BUFFER_SIZE 1024
#define MAX_SIZE_CHANNEL_NAME 21
#define MAX_SIZE_DEVICE_NAME 21

int get_channel(char *name);

int get_avr_name_conf(int serialPort);

int get_avr_channels_conf(int serialPort);

int get_name(char **args, int serialPort);

int set_name(char **args, int serialPort);

int set_channel_name(char **args, int serialPort);

int get_channel_value(char **args, int serialPort);

int set_channel_value(char **args, int serialPort);

int get_temperature(char **args, int serialPort);

int query_channel(char **args, int serialPort);

void free_memory(void);

#endif