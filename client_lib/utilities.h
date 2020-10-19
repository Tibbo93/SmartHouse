#ifndef UTILITIES_H
#define UTILITIES_H

#define MAX_NUM_ARGS 5
#define SWITCH_B 0
#define SWITCH_L 1
#define DIGITAL_IN 2
#define ANALOG_IN 3

int copy_channel(char *token, char **channels, char **def, int idx);

int perform(char buffer[], int serialPrt);

int download_conf(int serialPort);

int help();

#endif