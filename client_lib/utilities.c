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

int (*funs[])(char **) = {
    get_name,
    set_name,
    set_channel_name,
    get_channel_value,
    set_channel_value,
    get_temperature,
    query_channel,
    help
};

char *funs_name[] = {
    "get_name",
    "set_name",
    "set_channel_name",
    "get_channel_value",
    "set_channel_value",
    "get_temperature",
    "query_channel",
    "help",
    "exit"
};

int perform(char buffer[], int fd) {

    char *token;
    char **params;
    int status = -1, idx = 0, num_funs;

    serialPort = fd;
    memset(readBuffer, 0, sizeof(readBuffer));

    params = malloc(MAX_NUM_ARGS * sizeof(char *));
    if (params == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }

    token = strtok(buffer, " \n");
    while (token != NULL) {
        params[idx] = malloc((strlen(token) + 1) * sizeof(char));
        if (params[idx] == NULL) {
            printf("Error during allocation memory\n");
            return EXIT_FAILURE;
        }
        strcpy(params[idx], token);
        token = strtok(NULL, " \n");
        idx++;
    }
    params[idx] = NULL;

    num_funs = sizeof(funs) / sizeof(char *);

    if (params[0] != NULL) {
        for (int i = 0; i < num_funs; i++) {
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

int get_conf(int serial_port) {

    char *token, str[] = "get_conf";
    int numReadBytes = 0, idx = 0;

    memset(readBuffer, 0, sizeof(readBuffer));

    write(serialPort, str, strlen(str));
    usleep(1000000);

    numReadBytes = read(serialPort, readBuffer, sizeof(readBuffer));
    if (numReadBytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    token = strtok(readBuffer, " \n");
    name = malloc((strlen(token) + 1) * sizeof(char));
    if (name == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    strcpy(name, token);
    token = strtok(NULL, " \n");

    while (token != NULL) {

        if (strcmp(token, "0") == 0) {
            channels[idx] = malloc((strlen(default_digital_channels_name[idx]) + 1) * sizeof(char));
            if (channels[idx] == NULL) {
                printf("Error during allocation memory\n");
                return EXIT_FAILURE;
            }
            strcpy(channels[idx], default_digital_channels_name[idx]);
        } else {
            channels[idx] = malloc((strlen(token) + 1) * sizeof(char));
            if (channels[idx] == NULL) {
                printf("Error during allocation memory\n");
                return EXIT_FAILURE;
            }
            strcpy(channels[idx], token);
        }

        idx++;
        token = strtok(NULL, " \n");
    }

    return EXIT_SUCCESS;
}

int get_name(char **args) {

    int numReadBytes = 0;

    if (args[1] != NULL) {
        printf("ERROR: invalid argument\n");
        return EXIT_FAILURE;
    }

    write(serialPort, args[0], strlen(args[0]));
    usleep(1000000);

    numReadBytes = read(serialPort, readBuffer, sizeof(readBuffer));

    if (numReadBytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if (!strcmp(readBuffer, "0"))
        printf("Missing name\n");
    else
        printf("Device name: %s\n", readBuffer);

    return EXIT_SUCCESS;
}

int set_name(char **args) {

    int numReadBytes = 0, str_len = 0;
    char *str;

    if (args[1] == NULL) {
        printf("ERROR: missing name\n");
        return EXIT_FAILURE;
    } else if (args[2] != NULL) {
        printf("ERROR: too many arguments\n");
        return EXIT_FAILURE;
    }

    strcpy(name, args[1]);

    str_len = strlen(args[0]) + strlen(args[1]) + 1;
    str = malloc((str_len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %s", args[0], args[1]);
    write(serialPort, str, str_len);
    usleep(1000000);

    /*write(serialPort, args[0], strlen(args[0]));
    write(serialPort, " ", 1);
    write(serialPort, args[1], strlen(args[1]));
    usleep(1000000);*/

    return EXIT_SUCCESS;
}

int set_channel_name(char **args) {

    int i = 0, str_len = 0;
    char *str;

    if (args[1] == NULL || args[2] == NULL || args[3] == NULL)
        return EXIT_FAILURE;

    if (args[4] != NULL)
        return EXIT_FAILURE;

    if (strlen(args[3]) > MAX_SIZE_CHANNEL_NAME) {
        printf("Channel name too long (max 20 characters)\n");
        return EXIT_FAILURE;
    }

    if (strcmp(args[1], name) != 0) {
        printf("Device name not found\n");
        return EXIT_FAILURE;
    }

    for (; i < NUM_DIGITAL_CHANNELS; i++) {
        if (strcmp(args[2], channels[i]) == 0) {
            break;
        }
    }

    if (i == NUM_DIGITAL_CHANNELS) {
        printf("Channel name not found\n");
        return EXIT_FAILURE;
    }

    strcpy(channels[i], args[3]);

    str_len = strlen(args[0]) + strlen(args[3]) + 4;
    str = malloc((str_len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %d %s", args[0], i, args[3]);
    write(serialPort, str, strlen(str));
    usleep(1000000);

    free(str);

    return EXIT_SUCCESS;
}

int get_channel_value(char **args) {}

int set_channel_value(char **args) {}

int help() {

    int num_funs = sizeof(funs_name) / sizeof(char *);

    printf(">> Command list:\n");

    for (int i = 0; i < num_funs; i++) {
        printf("\t- %s\n", funs_name[i]);
    }

    return EXIT_SUCCESS;
}

int query_channel(char **args) {

    if (args[1] != NULL)
        return EXIT_FAILURE;

    printf("DIGITAL CHANNELS:\n");

    for (int i = 0; i < NUM_DIGITAL_CHANNELS; i++) {
        printf("\t- %s\n", channels[i]);
    }

    return EXIT_SUCCESS;
}

int get_temperature(char **args) {

    int num_read_bytes;
    char *token;

    write(serialPort, args[0], strlen(args[0]));
    usleep(1000000);

    num_read_bytes = read(serialPort, &readBuffer, sizeof(readBuffer));
    if (num_read_bytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    token = strtok(readBuffer, " \n");
    printf("Temperature: %s C\n", token);

    token = strtok(NULL, " \n");
    printf("Humidity: %s%%\n", token);

    return EXIT_SUCCESS;
}

void free_memory() {

    free(name);
    for (int i = 0; i < NUM_DIGITAL_CHANNELS; i++) {
        free(channels[i]);
    }
}