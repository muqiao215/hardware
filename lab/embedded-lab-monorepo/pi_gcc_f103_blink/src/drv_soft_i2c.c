#include "stm32f10x.h"
#include "bsp_delay.h"
#include "drv_soft_i2c.h"

static void drv_soft_i2c_scl(uint8_t level) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)level);
}

static void drv_soft_i2c_sda(uint8_t level) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)level);
}

void drv_soft_i2c_init(void) {
    GPIO_InitTypeDef gpio = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    drv_soft_i2c_scl(1);
    drv_soft_i2c_sda(1);
}

void drv_soft_i2c_start(void) {
    drv_soft_i2c_sda(1);
    drv_soft_i2c_scl(1);
    bsp_delay_short();
    drv_soft_i2c_sda(0);
    bsp_delay_short();
    drv_soft_i2c_scl(0);
}

void drv_soft_i2c_stop(void) {
    drv_soft_i2c_sda(0);
    drv_soft_i2c_scl(1);
    bsp_delay_short();
    drv_soft_i2c_sda(1);
}

void drv_soft_i2c_send_byte(uint8_t byte) {
    uint8_t bit;

    for (bit = 0; bit < 8; bit++) {
        drv_soft_i2c_sda((byte & 0x80U) != 0U);
        byte <<= 1;
        bsp_delay_short();
        drv_soft_i2c_scl(1);
        bsp_delay_short();
        drv_soft_i2c_scl(0);
    }

    drv_soft_i2c_sda(1);
    bsp_delay_short();
    drv_soft_i2c_scl(1);
    bsp_delay_short();
    drv_soft_i2c_scl(0);
}
