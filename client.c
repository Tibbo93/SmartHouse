#include "client_lib/serial.h"
#include "client_lib/utilities.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define PORT_PATH "/dev/ttyACM0"
#define BUFFER_SIZE 128

int serialPort;

int main(int argc, char const *argv[]) {

    int res;
    char buffer[BUFFER_SIZE] = "get_conf";

    serialPort = open(PORT_PATH, O_RDWR | O_NOCTTY | O_SYNC);
    usleep(1000000);

    if (serialPort < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    if (serial_port_init(serialPort) == -1) {
        printf("An error occured while communicating with the host");
        return EXIT_FAILURE;
    }

    res = download_conf(serialPort);
    if (res) {
        printf("Error during upload configuration");
        return EXIT_FAILURE;
    }

    printf("Loading configuration complete\n");
    printf("\nType 'help' to get command list\n");
    printf("Please insert one command at a time\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        printf("\n>> smart_house_host ");
        fgets(buffer, BUFFER_SIZE, stdin);

        if (!strcmp(buffer, "exit\n")) {
            break;
        }

        res = perform(buffer, serialPort);

        if (res == -1)
            printf("Command not found");
        else if (res == EXIT_FAILURE)
            printf("Invalid command");
        else
            printf("\nOperation complete!");
    }

    free_memory();
    close(serialPort);
    return EXIT_SUCCESS;
}
