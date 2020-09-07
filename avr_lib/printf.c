#include "printf.h"

#include "uart.h"
#include <stdio.h>

int usart_putchar_printf(char var, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

// this function is called by printf as a stream handler
int usart_putchar_printf(char var, FILE *stream) {

    if (var == '\n')
        uart_putc('\r');          //translate \n to \r for br@y++ terminal
    uart_putc(var);
    return 0;
}

void printf_init(void) {
    stdout = &mystdout;
}