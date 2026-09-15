#include "usart_cli_port.h"

#include "board.h"

void usart_cli_port_init(uint32_t baud_rate) {
    GPIO_InitTypeDef gpio = {0};
    USART_InitTypeDef usart = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    gpio.GPIO_Pin = BOARD_USART_TX_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BOARD_USART_GPIO, &gpio);

    gpio.GPIO_Pin = BOARD_USART_RX_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(BOARD_USART_GPIO, &gpio);

    usart.USART_BaudRate = baud_rate;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

int usart_cli_port_try_read_char(char *ch) {
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != SET) {
        return 0;
    }

    *ch = (char)USART_ReceiveData(USART1);
    return 1;
}

void usart_cli_port_write_char(char ch) {
    USART_SendData(USART1, (uint16_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {
    }
}

void usart_cli_port_write_string(const char *text) {
    while (*text != '\0') {
        usart_cli_port_write_char(*text++);
    }
}
