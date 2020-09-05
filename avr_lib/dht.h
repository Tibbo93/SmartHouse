#ifndef _DHT_H
#define DHT_H

#include <stdint.h>

void dht_init(void);
void dht_reset(void);
int8_t dht_read(int8_t *temperature, int8_t *humidity);

#endif