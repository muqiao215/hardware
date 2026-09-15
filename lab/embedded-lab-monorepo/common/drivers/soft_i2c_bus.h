#ifndef SOFT_I2C_BUS_H
#define SOFT_I2C_BUS_H

#include <stdint.h>

void soft_i2c_bus_init(void);
void soft_i2c_bus_write(uint8_t address, const uint8_t *data, uint16_t length);

#endif
