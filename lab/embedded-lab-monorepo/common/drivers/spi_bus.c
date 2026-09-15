#include "spi_bus.h"

#include "board.h"

void spi_bus_init(void) {
    GPIO_InitTypeDef gpio = {0};
    SPI_InitTypeDef spi = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | BOARD_SPI_CLOCK, ENABLE);

    gpio.GPIO_Pin = BOARD_SPI_SCK_PIN | BOARD_SPI_MOSI_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BOARD_SPI_GPIO, &gpio);

    gpio.GPIO_Pin = BOARD_SPI_MISO_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(BOARD_SPI_GPIO, &gpio);

    gpio.GPIO_Pin = BOARD_SPI_NSS_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(BOARD_SPI_GPIO, &gpio);
    GPIO_SetBits(BOARD_SPI_GPIO, BOARD_SPI_NSS_PIN);

    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    spi.SPI_CPHA = SPI_CPHA_1Edge;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CRCPolynomial = 7;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI1, &spi);
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t spi_bus_transfer(uint8_t value) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
    }
    SPI_I2S_SendData(SPI1, value);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
    }
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

void spi_bus_select(void) {
    GPIO_ResetBits(BOARD_SPI_GPIO, BOARD_SPI_NSS_PIN);
}

void spi_bus_deselect(void) {
    GPIO_SetBits(BOARD_SPI_GPIO, BOARD_SPI_NSS_PIN);
}
