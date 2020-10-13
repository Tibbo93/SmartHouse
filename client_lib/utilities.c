#include "utilities.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int serialPort;
char readBuffer[READ_BUFFER_SIZE];
char *name;
char *switch_B[8];
char *switch_L[8];
char *digital_in[8];
char *analog_in[8];

//switch pattern "name:pin_number"
char *default_switch_B[] = {
    "led_0",            //pin 53
    "led_1_r",          //pin 52
    "led_1_g",          //pin 51
    "led_1_b",          //pin 50
    "led_2_r",          //pin 10
    "led_2_g",          //pin 11
    "led_2_b",          //pin 12
    "led_3"            //pin 13
};

//switch pattern "name:pin_number"
char *default_switch_L[] = {
    "buzzer_0",           //pin 49
    "buzzer_1",           //pin 48
    "switch_11",          //pin 47
    "switch_12",          //pin 46
    "switch_13",          //pin 45
    "switch_14",          //pin 44
    "switch_15",          //pin 43
    "switch_16"           //pin 42
};

//digital_in pattern "name:pin_number"
char *default_digital_in[] = {
    "temperature",           //oin 37
    "motion",                //pin 36
    "fire",                  //pin 35
    "rain",                  //pin 34
    "digital_in_4",          //pin 33
    "digital_in_5",          //pin 32
    "digital_in_6",          //pin 31
    "digital_in_7"           //pin 30
};

//analog_in pattern "name"
char *default_analog_in[] = {
    "analog_0",
    "analog_1",
    "analog_2",
    "analog_3",
    "analog_4",
    "analog_5",
    "analog_6",
    "analog_7"
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

bool get_channel(char *old_name, char *new_name, char **channels, int *ch, int *count) {

    *ch = 0;
    (*count)++;

    for (; *ch < 8; (*ch)++) {
        if (strcmp(old_name, channels[*ch]) == 0) {
            channels[*ch] = realloc(channels[*ch], strlen(new_name) + 1);
            if (channels[*ch] == NULL) {
                printf("Error during allocation memory\n");
                return false;
            }
            strcpy(channels[*ch], new_name);
            return true;
        }
    }

    return false;
}

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

int download_conf(int fd) {

    serialPort = fd;

    if (get_avr_name_conf())
        return EXIT_FAILURE;

    if (get_avr_channels_conf())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int get_avr_name_conf(void) {

    char str[] = "get_avr_name";
    int numReadBytes;

    memset(readBuffer, 0, sizeof(readBuffer));

    write(serialPort, str, strlen(str));
    usleep(1000000);

    numReadBytes = read(serialPort, readBuffer, READ_BUFFER_SIZE);
    if (numReadBytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    readBuffer[numReadBytes] = '\0';

    name = malloc((numReadBytes + 1) * sizeof(char));
    if (name == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    strcpy(name, readBuffer);

    return EXIT_SUCCESS;
}

int get_avr_channels_conf() {

    char *token, str[] = "get_avr_channels";
    int numReadBytes, idx = 0, count = 0;

    memset(readBuffer, 0, sizeof(readBuffer));

    write(serialPort, str, strlen(str));
    usleep(1000000);

    numReadBytes = read(serialPort, readBuffer, READ_BUFFER_SIZE);
    if (numReadBytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    readBuffer[numReadBytes] = '\0';

    token = strtok(readBuffer, " \n");
    while (token != NULL) {
        switch (count) {
        case 0:
            if (copy_channel(token, switch_B, default_switch_B, idx))
                return EXIT_FAILURE;
            break;
        case 1:
            if (copy_channel(token, switch_L, default_switch_L, idx))
                return EXIT_FAILURE;
            break;
        case 2:
            if (copy_channel(token, digital_in, default_digital_in, idx))
                return EXIT_FAILURE;
            break;
        case 3:
            if (copy_channel(token, analog_in, default_analog_in, idx))
                return EXIT_FAILURE;
            break;
        default:
            break;
        }

        idx++;
        token = strtok(NULL, " \n");
        if ((idx % 8) == 0) {
            count++;
            idx = 0;
        }
    }

    return EXIT_SUCCESS;
}

int get_name(char **args) {

    if (args[1] != NULL) {
        printf("ERROR: invalid argument\n");
        return EXIT_FAILURE;
    }

    if (strcmp(name, "0") == 0)
        printf("\n\tMissing name\n");
    else
        printf("\n\tDevice name: %s\n", name);

    return EXIT_SUCCESS;
}

int set_name(char **args) {

    int numReadBytes = 0, len = 0;
    char *str;

    if (args[1] == NULL) {
        printf("ERROR: missing name\n");
        return EXIT_FAILURE;
    } else if (args[2] != NULL) {
        printf("ERROR: too many arguments\n");
        return EXIT_FAILURE;
    }

    strcpy(name, args[1]);

    len = strlen(args[0]) + strlen(args[1]) + 1;
    str = malloc((len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %s", args[0], args[1]);
    write(serialPort, str, len);
    usleep(1000000);

    return EXIT_SUCCESS;
}

int set_channel_name(char **args) {

    int ch, count = -1, len;
    char *str;
    bool found = false;

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

    found = get_channel(args[2], args[3], switch_B, &ch, &count);

    if (!found)
        found = get_channel(args[2], args[3], switch_L, &ch, &count);

    if (!found)
        found = get_channel(args[2], args[3], digital_in, &ch, &count);

    if (!found)
        found = get_channel(args[2], args[3], analog_in, &ch, &count);

    if (!found) {
        printf("Channel name not found\n");
        return EXIT_FAILURE;
    }

    ch = ch + (8 * count);
    len = strlen(args[0]) + strlen(args[3]) + 5;
    str = malloc((len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %d %s", args[0], ch, args[3]);
    write(serialPort, str, len);
    usleep(5000000);

    free(str);
    return EXIT_SUCCESS;
}

int get_channel_value(char **args) {}

int set_channel_value(char **args) {

    int ch = 0, value, str_len;
    char *str;

    /*if (args[1] == NULL || args[2] == NULL || args[3] == NULL)
        return EXIT_FAILURE;

    if (args[1] != name)
        return EXIT_FAILURE;

    for (; ch < NUM_DIGITAL_CHANNELS; ch++) {
        if (strcmp(args[2], channels[ch]) == 0)
            break;
    }

    if (ch == NUM_DIGITAL_CHANNELS) {
        printf("Channel name not found\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < strlen(args[3]); i++) {
        if (isdigit(args[3][i]) == 0) {
            printf("Invalid value");
            return EXIT_FAILURE;
        }
    }

    value = atoi(args[3]);

    if (value < 0 || value > 255) {
        printf("Invalid value");
        return EXIT_FAILURE;
    }

    str_len = strlen(args[0]) + strlen(args[3]) + 4;
    str = malloc((str_len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %d %s", args[0], ch, args[3]);
    write(serialPort, str, strlen(str));
    usleep(1000000);

    free(str);*/
    return EXIT_SUCCESS;
}

int help() {

    int num_funs = sizeof(funs_name) / sizeof(char *);

    printf("\n\tCommand list:\n");

    for (int i = 0; i < num_funs; i++) {
        printf("\t\t- %s\n", funs_name[i]);
    }

    return EXIT_SUCCESS;
}

int query_channel(char **args) {

    if (args[1] != NULL)
        return EXIT_FAILURE;

    printf("\n\tSWITCH CHANNELS:\n");
    for (int i = 0; i < 8; i++) {
        printf("\t\t- %s\n", switch_B[i]);
    }
    for (int i = 0; i < 8; i++) {
        printf("\t\t- %s\n", switch_L[i]);
    }

    printf("\n\tDIGITAL INPUT CHANNELS:\n");
    for (int i = 0; i < 8; i++) {
        printf("\t\t- %s\n", digital_in[i]);
    }

    printf("\n\tANALOG INPUT CHANNELS:\n");
    for (int i = 0; i < 8; i++) {
        printf("\t\t- %s\n", analog_in[i]);
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
    printf("\t> Temperature: %s C\n", token);

    token = strtok(NULL, " \n");
    printf("\t> Humidity: %s%%\n", token);

    return EXIT_SUCCESS;
}

void free_memory(void) {

    free(name);

    for (int i = 0; i < 8; i++) {
        free(switch_B[i]);
        free(switch_L[i]);
        free(digital_in[i]);
        free(analog_in[i]);
    }
}