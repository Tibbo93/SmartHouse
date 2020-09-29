#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>

#define MAX_SIZE_NAME 22
#define NUM_DEVICES 8
#define NUM_RGB_LEDS 3

/*******************************************   
 *           Setup Data Structures           
 * ****************************************/

typedef struct {
    char name[MAX_SIZE_NAME];
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_rgb;

typedef struct {
    char name[MAX_SIZE_NAME];
    uint8_t pin;
} device;

typedef struct {
    char name[MAX_SIZE_NAME];
    uint8_t rs;
    uint8_t rw;
    uint8_t e;
    uint8_t d4;
    uint8_t d5;
    uint8_t d6;
    uint8_t d7;
} lcd;

typedef struct {
    uint8_t status;
    char name[MAX_SIZE_NAME];
    led_rgb RGBLed[NUM_RGB_LEDS];
    device devices[NUM_DEVICES];
    lcd display;
} controller;

/*******************************************
 *           Setup Functions           
 * ****************************************/

controller startSetup(void);

int get_setup(int serialPort, controller *c);

void delay(int milliseconds);

#endif