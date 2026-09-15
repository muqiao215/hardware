#ifndef DRV_SSD1306_H
#define DRV_SSD1306_H

#include <stdint.h>

void drv_ssd1306_write_command(uint8_t command);
void drv_ssd1306_write_data(uint8_t data);
void drv_ssd1306_set_cursor(uint8_t page, uint8_t column);

#endif
