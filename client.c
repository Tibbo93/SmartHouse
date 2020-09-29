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

    char buffer[BUFFER_SIZE];

    serialPort = open(PORT_PATH, O_RDWR);

    if (serialPort < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    if (serial_port_init(serialPort) == -1) {
        printf("An error occured while communicating with the host");
        return EXIT_FAILURE;
    }

    printf("\nType 'help' to get command list\n");
    printf("Please insert one command at a time\n");

    do {
        memset(buffer, 0, sizeof(buffer));
        printf("\n>> smart_house_host ");
        fgets(buffer, BUFFER_SIZE, stdin);

        perform(buffer, serialPort);
    } while (strcmp(buffer, "exit"));

    close(serialPort);
    return 0;
}
