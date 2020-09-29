#include "utilities.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int serialPort;
char readBuffer[READ_BUFFER_SIZE];

int (*funs[NUM_FUNS])(char **) = {
    get_name,
    set_name,
    get_channel_name,
    set_channel_name,
    get_channel_value,
    set_channel_value,
    help,
    get_temperature,
    query_channel
};

char *funs_name[NUM_FUNS] = {
    "get_name",
    "set_name",
    "get_channel_name",
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
        params[idx++] = token;
        token = strtok(NULL, " \n");
    }
    params[idx] = NULL;

    if (params[0] != NULL) {
        for (int i = 0; i < NUM_FUNS; i++) {
            if (strcmp(params[0], funs_name[i]) == 0)
                status = funs[i](params);
        }
    }
    
    free(params);
    return status;
}

int get_name(char **args) {

    int numReadBytes = 0;

    if (args[1] != NULL) {
        printf("ERROR: invalid argument");
        return -1;
    }

    write(serialPort, args[0], sizeof(args[0]));
    usleep(1000000);

    numReadBytes = read(serialPort, &readBuffer, sizeof(readBuffer));

    if (numReadBytes < 0)
        printf("Error reading: %s\n", strerror(errno));

    if (!strcmp(readBuffer, "0"))
        printf("Missing name\n");
    else
        printf("%s", readBuffer);

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

int get_channel_name(char **args) {}

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

int query_channel(char **args) {}

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