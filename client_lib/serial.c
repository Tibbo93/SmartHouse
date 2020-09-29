#include "serial.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

struct termios tty;

int serial_port_init(int serialPort) {

    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(serialPort, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    tty.c_cflag &= ~PARENB;                 //clear parity bit, disabling parity
    tty.c_cflag &= ~CSTOPB;                 //clear stop field, only one stop bit used in communication
    tty.c_cflag |= CS8;                     //8 bits per byte
    tty.c_cflag &= ~CRTSCTS;                //disable RTS/CTS hardware flow control
    tty.c_cflag |= CREAD | CLOCAL;          //turn on read & ignore ctrl lines

    tty.c_lflag &= ~ICANON;          //disable canonic mode
    tty.c_lflag &= ~ECHO;            //disable echo
    tty.c_lflag &= ~ECHOE;           //disable erasure
    tty.c_lflag &= ~ECHONL;          //disable new-line echo
    tty.c_lflag &= ~ISIG;            //disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                               //turn off sw flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);          //disable any special handling of received bytes

    tty.c_oflag &= ~OPOST;          //prevent special interpretation of output bytes
    tty.c_oflag &= ~ONLCR;          //prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 5;          //wait for up to 1s, returning as soon as any data is received
    tty.c_cc[VMIN] = 0;

    //set in/out baud rate
    cfsetispeed(&tty, B19200);
    cfsetospeed(&tty, B19200);

    //save tty settings, also checking for error
    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    return 0;
}