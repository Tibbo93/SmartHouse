#include "utilities.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int serialPort;
char readBuffer[READ_BUFFER_SIZE];
char *name;
char *channels[NUM_DIGITAL_CHANNELS];

char *default_digital_channels_name[NUM_DIGITAL_CHANNELS] = {
    "pin_4",
    "pin_7",
    "pin_8",
    "pin_10",
    "pin_11",
    "pin_12",
    "pin_22",
    "pin_23",
    "pin_24",
    "pin_25",
    "pin_26",
    "pin_27",
    "pin_28",
    "pin_34",
    "pin_50",
    "pin_52",
    "pin_53",
};

int (*funs[NUM_FUNS])(char **) = {
    get_conf,
    get_name,
    set_name,
    set_channel_name,
    get_channel_value,
    set_channel_value,
    help,
    get_temperature,
    query_channel
};

char *funs_name[NUM_FUNS] = {
    "get_conf",
    "get_name",
    "set_name",
    "set_channel_name",
    "get_channel_value",
    "set_channel_value",
    "help",
    "get_temperature",
    "query_channel"
};

int perform(char buffer[], int fd) {

    char *token;
    char **params;
    int status, idx = 0;

    serialPort = fd;
    memset(readBuffer, 0, sizeof(readBuffer));

    params = malloc(MAX_NUM_ARGS * sizeof(char *));
    if (params == NULL) {
        printf("Error during allocation memory\n");
        return -1;
    }

    token = strtok(buffer, " \n");
    while (token != NULL) {
        params[idx] = malloc((strlen(token) + 1) * sizeof(char));
        if (params[idx] == NULL) {
            printf("Error during allocation memory\n");
            return -1;
        }
        strcpy(params[idx], token);
        token = strtok(NULL, " \n");
        idx++;
    }
    params[idx] = NULL;

    if (params[0] != NULL) {
        for (int i = 0; i < NUM_FUNS; i++) {
            if (strcmp(params[0], funs_name[i]) == 0) {
                status = funs[i](params);
            }
        }
    }

    for (int i = 0; i < idx; i++) {
        free(params[i]);
    }
    free(params);

    return status;
}

int get_conf(char **args) {

    char *token;
    int numReadBytes = 0, idx = 0;

    memset(readBuffer, 0, sizeof(readBuffer));

    write(serialPort, args[0], strlen(args[0]));
    usleep(1000000);

    numReadBytes = read(serialPort, readBuffer, sizeof(readBuffer));
    if (numReadBytes < 0)
        printf("Error reading: %s\n", strerror(errno));

    token = strtok(readBuffer, " \n");
    name = malloc((strlen(token) + 1) * sizeof(char));
    if (name == NULL) {
        printf("Error during allocation memory\n");
        return -1;
    }
    strcpy(name, token);
    token = strtok(NULL, " \n");

    while (token != NULL) {
        channels[idx] = malloc((strlen(token) + 1) * sizeof(char));
        if (channels[idx] == NULL) {
            printf("Error during allocation memory\n");
            return -1;
        }
        strcpy(channels[idx++], token);
        token = strtok(NULL, " \n");
    }

    return 1;
}

int get_name(char **args) {

    int numReadBytes = 0;

    if (args[1] != NULL) {
        printf("ERROR: invalid argument");
        return -1;
    }

    write(serialPort, args[0], strlen(args[0]));
    usleep(1000000);

    numReadBytes = read(serialPort, readBuffer, sizeof(readBuffer));

    if (numReadBytes < 0)
        printf("Error reading: %s\n", strerror(errno));

    if (!strcmp(readBuffer, "0"))
        printf("Missing name\n");
    else
        printf("Device name: %s\n", readBuffer);

    return 1;
}

int set_name(char **args) {

    int numReadBytes = 0;

    if (args[1] == NULL) {
        printf("ERROR: missing name");
        return -1;
    } else if (args[2] != NULL) {
        printf("ERROR: too many arguments");
        return -1;
    }

    write(serialPort, args[0], strlen(args[0]));
    write(serialPort, " ", 1);
    write(serialPort, args[1], strlen(args[1]));
    usleep(1000000);

    return 1;
}

int set_channel_name(char **args) {}

int get_channel_value(char **args) {}

int set_channel_value(char **args) {}

int help() {

    printf(">> Command list:\n");

    for (int i = 0; i < NUM_FUNS; i++) {
        printf("\t- %s\n", funs_name[i]);
    }

    return 1;
}

int query_channel(char **args) {

    if (args[1] != NULL)
        return -1;

    printf("DIGITAL CHANNELS:\n");

    for (int i = 0; i < NUM_DIGITAL_CHANNELS; i++) {
        if (strcmp(channels[i], "3") == 0)
            printf("\t- %s \t\t (default name)\n", default_digital_channels_name[i]);
        else
            printf("\t- %s\n", channels[i]);
    }

    return 1;
}

int get_temperature(char **args) {

    int n;
    char *token;

    write(serialPort, args[0], strlen(args[0]));
    usleep(1000000);

    n = read(serialPort, &readBuffer, sizeof(readBuffer));
    if (n < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return -1;
    }

    token = strtok(readBuffer, " \n");
    printf("Temperature: %s C\n", token);

    token = strtok(NULL, " \n");
    printf("Humidity: %s%%\n", token);

    return 1;
}