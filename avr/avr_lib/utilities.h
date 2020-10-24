#ifndef UTILITIES_H
#define UTILITIES_H

#include <avr/eeprom.h>
#include <stdint.h>

#define UART_BAUDRATE 19200
#define RX_BUFFER_SIZE 128
#define MAX_SIZE_DEVICE_NAME 21
#define MAX_SIZE_CHANNEL_NAME 21
#define NUM_CHANNELS 32
#define NUM_ARGS 4
#define NUM_FUNS 7
#define MSG_SIZE_NO_DATA 9

#define REQUEST_MSG 1
#define REPLY_MSG 2
#define ERROR_MSG 3

#define SWITCH_B 0
#define SWITCH_L 1
#define DIGITAL_IN 2
#define ANALOG_IN 3

#define MOTION_SENS_PIN PC1
#define MOTION_LED_PIN PB7
#define MOTION_BUZZER_PIN PL1

#define FIRE_SENS_PIN PC2
#define FIRE_LED_PIN PB0
#define FIRE_BUZZER_PIN PL0

typedef struct {
    uint8_t nameSet;
    char name[MAX_SIZE_CHANNEL_NAME];
} Channel;

void digital_out_init(void);

void digital_in_init(void);

void adc_init(void);

uint16_t adc_read(uint8_t ch);

void pwm_init(void);

void uart_init(void);

uint8_t get_request(char *buff);

uint8_t perform_request(char *buffer);

void send_response(const uint8_t *buff, uint8_t len);

void handle_request(char *buff, int recvBytes);

void detect_motion(void);

void detect_fire(void);

#endif
