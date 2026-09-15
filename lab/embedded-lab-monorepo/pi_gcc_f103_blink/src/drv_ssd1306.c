#include "drv_ssd1306.h"

#include "drv_soft_i2c.h"

#define SSD1306_I2C_ADDRESS 0x78U
#define SSD1306_CONTROL_COMMAND 0x00U
#define SSD1306_CONTROL_DATA 0x40U

void drv_ssd1306_write_command(uint8_t command) {
    drv_soft_i2c_start();
    drv_soft_i2c_send_byte(SSD1306_I2C_ADDRESS);
    drv_soft_i2c_send_byte(SSD1306_CONTROL_COMMAND);
    drv_soft_i2c_send_byte(command);
    drv_soft_i2c_stop();
}

void drv_ssd1306_write_data(uint8_t data) {
    drv_soft_i2c_start();
    drv_soft_i2c_send_byte(SSD1306_I2C_ADDRESS);
    drv_soft_i2c_send_byte(SSD1306_CONTROL_DATA);
    drv_soft_i2c_send_byte(data);
    drv_soft_i2c_stop();
}

void drv_ssd1306_set_cursor(uint8_t page, uint8_t column) {
    drv_ssd1306_write_command(0xB0U | page);
    drv_ssd1306_write_command(0x10U | ((column & 0xF0U) >> 4));
    drv_ssd1306_write_command(0x00U | (column & 0x0FU));
}
