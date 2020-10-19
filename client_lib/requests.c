#include "requests.h"
#include "utilities.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char readBuffer[READ_BUFFER_SIZE];
char *name;
char *switch_B[8];
char *switch_L[8];
char *digital_in[8];
char *analog_in[8];

char *default_switch_B[] = {
    "led_0",            //pin 53
    "led_1_r",          //pin 52
    "led_1_g",          //pin 51
    "led_1_b",          //pin 50
    "led_2_r",          //pin 10
    "led_2_g",          //pin 11
    "led_2_b",          //pin 12
    "led_3"             //pin 13
};

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

char *default_digital_in[] = {
    "temperature",           //pin 37
    "motion",                //pin 36
    "fire",                  //pin 35
    "rain",                  //pin 34
    "digital_in_4",          //pin 33
    "digital_in_5",          //pin 32
    "digital_in_6",          //pin 31
    "digital_in_7"           //pin 30
};

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

int get_channel(char *name) {

    for (int i = 0; i < 8; i++) {
        if (strcmp(name, switch_B[i]) == 0) {
            return i;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (strcmp(name, switch_L[i]) == 0) {
            return i + 8;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (strcmp(name, digital_in[i]) == 0) {
            return i + 16;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (strcmp(name, analog_in[i]) == 0) {
            return i + 24;
        }
    }

    return -1;
}

int get_avr_name_conf(int serialPort) {

    char str[] = "get_avr_name";
    int numReadBytes;

    memset(readBuffer, 0, sizeof(readBuffer));

    write(serialPort, str, sizeof(str));
    usleep(500000);

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

int get_avr_channels_conf(int serialPort) {

    char *token, str[] = "get_avr_channels";
    int numReadBytes, idx = 0, count = 0;

    memset(readBuffer, 0, sizeof(readBuffer));

    write(serialPort, str, sizeof(str));
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

int get_name(char **args, int serialPort) {

    if (args[1] != NULL) {
        printf("ERROR: invalid argument\n");
        return EXIT_FAILURE;
    }

    if (strcmp(name, "0") == 0)
        printf("\n\tMissing name\n");
    else
        printf("\n\tDEVICE NAME: %s\n", name);

    return EXIT_SUCCESS;
}

int set_name(char **args, int serialPort) {

    int numReadBytes = 0, len = 0;
    char *str;

    if (args[1] == NULL) {
        printf("ERROR: missing name\n");
        return EXIT_FAILURE;
    } else if (args[2] != NULL) {
        printf("ERROR: too many arguments\n");
        return EXIT_FAILURE;
    } else if (strlen(args[1]) > MAX_SIZE_DEVICE_NAME - 1) {
        printf("Name too long, max 20 characters");
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

int set_channel_name(char **args, int serialPort) {

    int ch, count = -1, len;
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

    ch = get_channel(args[2]);

    if (ch == -1) {
        printf("Channel name not found\n");
        return EXIT_FAILURE;
    }

    int type = ch / 8, bit = ch % 8;

    switch (type) {
    case 0:
        switch_B[bit] = realloc(switch_B[bit], strlen(args[3]) + 1);
        if (switch_B[bit] == NULL) {
            printf("Error during allocation memory\n");
            return EXIT_FAILURE;
        }
        strcpy(switch_B[bit], args[3]);
        break;
    case 1:
        switch_L[bit] = realloc(switch_L[bit], strlen(args[3]) + 1);
        if (switch_L[bit] == NULL) {
            printf("Error during allocation memory\n");
            return EXIT_FAILURE;
        }
        strcpy(switch_L[bit], args[3]);
        break;
    case 2:
        digital_in[bit] = realloc(digital_in[bit], strlen(args[3]) + 1);
        if (digital_in[bit] == NULL) {
            printf("Error during allocation memory\n");
            return EXIT_FAILURE;
        }
        strcpy(digital_in[bit], args[3]);
        break;
    case 3:
        analog_in[bit] = realloc(analog_in[bit], strlen(args[3]) + 1);
        if (analog_in[bit] == NULL) {
            printf("Error during allocation memory\n");
            return EXIT_FAILURE;
        }
        strcpy(analog_in[bit], args[3]);
        break;
    default:
        break;
    }

    len = strlen(args[0]) + strlen(args[3]) + 5;
    str = malloc((len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %d %s", args[0], ch, args[3]);
    write(serialPort, str, len);
    usleep(1000000);

    free(str);
    return EXIT_SUCCESS;
}

int get_channel_value(char **args, int serialPort) {

    int ch, len, num_read_bytes;
    char *str;

    if (args[1] == NULL)
        return EXIT_FAILURE;

    if (args[2] != NULL)
        return EXIT_FAILURE;

    ch = get_channel(args[1]);
    if (ch == -1) {
        printf("Channel name not found\n");
        return EXIT_FAILURE;
    }

    memset(readBuffer, 0, sizeof(readBuffer));

    len = strlen(args[0]) + 3;
    str = malloc((len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %d", args[0], ch);
    write(serialPort, str, len);
    usleep(1000000);

    num_read_bytes = read(serialPort, readBuffer, READ_BUFFER_SIZE);
    if (num_read_bytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    readBuffer[num_read_bytes] = '\0';
    printf("\n\tVALUE: %s\n", readBuffer);

    free(str);
    return EXIT_SUCCESS;
}

int set_channel_value(char **args, int serialPort) {

    int ch = 0, value, len;
    char *str;

    if (args[1] == NULL || args[2] == NULL)
        return EXIT_FAILURE;

    if (args[3] != NULL)
        return EXIT_FAILURE;

    ch = get_channel(args[1]);
    if (ch == -1) {
        printf("Channel name not found\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < strlen(args[2]); i++) {
        if (isdigit(args[2][i]) == 0) {
            printf("Invalid value");
            return EXIT_FAILURE;
        }
    }

    value = atoi(args[2]);

    if (value < 0 || value > 255) {
        printf("Invalid value");
        return EXIT_FAILURE;
    }

    len = strlen(args[0]) + strlen(args[2]) + 4;
    str = malloc((len + 1) * sizeof(char));
    if (str == NULL) {
        printf("Error during allocation memory\n");
        return EXIT_FAILURE;
    }
    sprintf(str, "%s %d %s", args[0], ch, args[2]);
    write(serialPort, str, len);
    usleep(1000000);

    free(str);
    return EXIT_SUCCESS;
}

int get_temperature(char **args, int serialPort) {

    int num_read_bytes;
    char *token;

    memset(readBuffer, 0, sizeof(readBuffer));

    write(serialPort, args[0], strlen(args[0]) + 1);
    usleep(1000000);

    num_read_bytes = read(serialPort, readBuffer, READ_BUFFER_SIZE);
    if (num_read_bytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    token = strtok(readBuffer, " \n");
    printf("\n\tTEMPERATURE: %s C\n", token);

    token = strtok(NULL, " \n");
    printf("\tHUMIDITY: %s%%\n", token);

    return EXIT_SUCCESS;
}

int query_channel(char **args, int serialPort) {

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

void free_memory(void) {

    free(name);

    for (int i = 0; i < 8; i++) {
        free(switch_B[i]);
        free(switch_L[i]);
        free(digital_in[i]);
        free(analog_in[i]);
    }
}