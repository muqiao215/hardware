#ifndef DRV_SOFT_I2C_H
#define DRV_SOFT_I2C_H

#include <stdint.h>

void drv_soft_i2c_init(void);
void drv_soft_i2c_start(void);
void drv_soft_i2c_stop(void);
void drv_soft_i2c_send_byte(uint8_t byte);

#endif
