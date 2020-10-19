#include "utilities.h"
#include "requests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int (*funs[])(char **, int) = {
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

char *funs_description[] = {
    "     Retrieve device name",
    "<device_name>    Set device name (max 20 characters)",
    "<device_name> <old_channel_name> <new_channel_name>     Set channel name (max 20 characters)",
    "<channel_name>     Retrieve channel value",
    "<channel_name> <value>     Set channel value (0 <= value <= 255)",
    "     Retrieve temperature and humidity by the sensor",
    "     List of supported channels",
    "     List of available commands",
    ""
};

int copy_channel(char *token, char **channels, char **def, int idx) {

    if (strcmp(token, "0") == 0) {
        channels[idx] = malloc((strlen(def[idx]) + 1) * sizeof(char));
        if (channels[idx] == NULL) {
            printf("Error during allocation memory\n");
            return EXIT_FAILURE;
        }
        strcpy(channels[idx], def[idx]);
    } else {
        channels[idx] = malloc((strlen(token) + 1) * sizeof(char));
        if (channels[idx] == NULL) {
            printf("Error during allocation memory\n");
            return EXIT_FAILURE;
        }
        strcpy(channels[idx], token);
    }

    return EXIT_SUCCESS;
}

int perform(char buffer[], int serialPort) {

    char *token;
    char **params;
    int status = -1, idx = 0, num_funs;

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
                status = funs[i](params, serialPort);
            }
        }
    }

    for (int i = 0; i < idx; i++) {
        free(params[i]);
    }
    free(params);

    return status;
}

int download_conf(int serialPort) {

    if (get_avr_name_conf(serialPort))
        return EXIT_FAILURE;

    if (get_avr_channels_conf(serialPort))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int help() {

    int num_funs = sizeof(funs_name) / sizeof(char *);

    printf("\n\tCommand list:\n");

    for (int i = 0; i < num_funs; i++) {
        printf("\t\t- %s %s\n", funs_name[i], funs_description[i]);
    }

    return EXIT_SUCCESS;
}