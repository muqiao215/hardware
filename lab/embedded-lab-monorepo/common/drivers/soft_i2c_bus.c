#include "soft_i2c_bus.h"

#include "board.h"
#include "delay.h"

static void soft_i2c_bus_scl(uint8_t level) {
    GPIO_WriteBit(BOARD_SOFT_I2C_GPIO, BOARD_SOFT_I2C_SCL_PIN, (BitAction)level);
}

static void soft_i2c_bus_sda(uint8_t level) {
    GPIO_WriteBit(BOARD_SOFT_I2C_GPIO, BOARD_SOFT_I2C_SDA_PIN, (BitAction)level);
}

static void soft_i2c_bus_start(void) {
    soft_i2c_bus_sda(1);
    soft_i2c_bus_scl(1);
    delay_short();
    soft_i2c_bus_sda(0);
    delay_short();
    soft_i2c_bus_scl(0);
}

static void soft_i2c_bus_stop(void) {
    soft_i2c_bus_sda(0);
    soft_i2c_bus_scl(1);
    delay_short();
    soft_i2c_bus_sda(1);
}

static void soft_i2c_bus_send_byte(uint8_t value) {
    uint8_t bit;

    for (bit = 0; bit < 8; bit++) {
        soft_i2c_bus_sda((value & 0x80U) != 0U);
        value <<= 1;
        delay_short();
        soft_i2c_bus_scl(1);
        delay_short();
        soft_i2c_bus_scl(0);
    }

    soft_i2c_bus_sda(1);
    delay_short();
    soft_i2c_bus_scl(1);
    delay_short();
    soft_i2c_bus_scl(0);
}

void soft_i2c_bus_init(void) {
    GPIO_InitTypeDef gpio = {0};

    RCC_APB2PeriphClockCmd(BOARD_SOFT_I2C_CLOCK, ENABLE);

    gpio.GPIO_Pin = BOARD_SOFT_I2C_SCL_PIN | BOARD_SOFT_I2C_SDA_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BOARD_SOFT_I2C_GPIO, &gpio);

    soft_i2c_bus_scl(1);
    soft_i2c_bus_sda(1);
}

void soft_i2c_bus_write(uint8_t address, const uint8_t *data, uint16_t length) {
    uint16_t index;

    soft_i2c_bus_start();
    soft_i2c_bus_send_byte(address);
    for (index = 0; index < length; index++) {
        soft_i2c_bus_send_byte(data[index]);
    }
    soft_i2c_bus_stop();
}
