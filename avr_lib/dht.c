#include "dht.h"
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

void dht_init(void) {

    DHT_PIN_OUTPUT();
    DHT_PIN_HIGH();
    _delay_ms(100);
}

void dht_reset(void) {
    DHT_PIN_OUTPUT();
    DHT_PIN_LOW();
    _delay_ms(100);
}

int8_t dht_read(int8_t *temperature, int8_t *humidity) {

    uint8_t data[5] = { 0, 0, 0, 0, 0 };

    //send request to the sensor
    DHT_PIN_LOW();          //set to low pin
    _delay_ms(20);
    DHT_PIN_HIGH();           //set to high pin
    DHT_PIN_INPUT();          //set DHT_PIN as input
    _delay_us(40);

    //check start condition 1
    if (DHT_PIN_READ())
        return -1;
    _delay_us(80);

    //check start condition 2
    if (!DHT_PIN_READ())
        return -1;
    _delay_us(80);

    //read data
    uint16_t timeOutCounter = 0;
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t result = 0;
        for (uint8_t j = 0; j < 8; j++) {
            timeOutCounter = 0;
            while (!DHT_PIN_READ()) {
                timeOutCounter++;
                if (timeOutCounter > DHT_TIMEOUT)
                    return -1;
            }
            _delay_us(30);
            if (DHT_PIN_READ())
                result |= (1 << (7 - j));
            timeOutCounter = 0;
            while (DHT_PIN_READ()) {
                timeOutCounter++;
                if (timeOutCounter > DHT_TIMEOUT) {
                    return -1;
                }
            }
        }
        data[i] = result;
    }

    _delay_ms(100);

    //check checksum
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4]) {
        *temperature = data[2];
        *humidity = data[0];
        return 0;
    }

    return -1;
}
