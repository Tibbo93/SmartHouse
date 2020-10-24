#ifndef _DHT_H
#define DHT_H

#define DHT_PIN PC0
#define DHT_TIMEOUT 200
#define DHT_PIN_INPUT() (DDRC &= ~(1 << DHT_PIN))
#define DHT_PIN_OUTPUT() (DDRC |= (1 << DHT_PIN))
#define DHT_PIN_LOW() (PORTC &= ~(1 << DHT_PIN))
#define DHT_PIN_HIGH() (PORTC |= (1 << DHT_PIN))
#define DHT_PIN_READ() (PINC & (1 << DHT_PIN))

#include <stdint.h>

void dht_init(void);

void dht_reset(void);

int8_t dht_read(int8_t *temperature, int8_t *humidity);

#endif