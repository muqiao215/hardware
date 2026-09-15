#include "i2c_bus.h"

#include "board.h"

void i2c_bus_init(uint32_t clock_hz, uint8_t own_address) {
    GPIO_InitTypeDef gpio = {0};
    I2C_InitTypeDef i2c = {0};

    RCC_APB2PeriphClockCmd(BOARD_I2C_CLOCK, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    gpio.GPIO_Pin = BOARD_I2C_SCL_PIN | BOARD_I2C_SDA_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BOARD_I2C_GPIO, &gpio);

    I2C_DeInit(I2C1);
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c.I2C_ClockSpeed = clock_hz;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_OwnAddress1 = own_address;
    I2C_Init(I2C1, &i2c);
    I2C_Cmd(I2C1, ENABLE);
}

int i2c_bus_probe(uint8_t address_7bit) {
    uint8_t address = (uint8_t)(address_7bit << 1);

    I2C_GenerateSTART(I2C1, ENABLE);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) {
    }

    I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET &&
           I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == RESET) {
    }

    if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {
        I2C_ClearFlag(I2C1, I2C_FLAG_AF);
        I2C_GenerateSTOP(I2C1, ENABLE);
        return 0;
    }

    (void)I2C1->SR1;
    (void)I2C1->SR2;
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 1;
}
