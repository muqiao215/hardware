#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <stdint.h>

void i2c_bus_init(uint32_t clock_hz, uint8_t own_address);
int i2c_bus_probe(uint8_t address_7bit);

#endif
