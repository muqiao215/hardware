#include "stm32f10x.h"
#include "Serial.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define SERIAL_RX_BUF_SIZE 256

static volatile uint8_t s_rx_buf[SERIAL_RX_BUF_SIZE];
static volatile uint16_t s_rx_head = 0;
static volatile uint16_t s_rx_tail = 0;

static void Serial_RxPush(uint8_t b)
{
    uint16_t next = (uint16_t)((s_rx_head + 1) % SERIAL_RX_BUF_SIZE);
    if (next == s_rx_tail)
    {
        return;
    }
    s_rx_buf[s_rx_head] = b;
    s_rx_head = next;
}

static int Serial_RxPop(uint8_t *out)
{
    if (s_rx_tail == s_rx_head)
    {
        return 0;
    }
    *out = s_rx_buf[s_rx_tail];
    s_rx_tail = (uint16_t)((s_rx_tail + 1) % SERIAL_RX_BUF_SIZE);
    return 1;
}

void Serial_Init(uint32_t baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
}

void Serial_SendString(const char *String)
{
    for (uint32_t i = 0; String[i] != '\0'; i++)
    {
        Serial_SendByte((uint8_t)String[i]);
    }
}

int fputc(int ch, FILE *f)
{
    (void)f;
    Serial_SendByte((uint8_t)ch);
    return ch;
}

void Serial_Printf(const char *format, ...)
{
    char buf[128];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);
    Serial_SendString(buf);
}

int Serial_ReadLine(char *out, uint16_t out_size)
{
    static char line_buf[128];
    static uint16_t line_len = 0;
    uint8_t b;

    while (Serial_RxPop(&b))
    {
        if (b == '\r')
        {
            continue;
        }
        if (b == '\n')
        {
            line_buf[line_len] = '\0';
            strncpy(out, line_buf, out_size);
            out[out_size - 1] = '\0';
            line_len = 0;
            return 1;
        }

        if (line_len < (uint16_t)(sizeof(line_buf) - 1))
        {
            line_buf[line_len++] = (char)b;
        }
        else
        {
            line_len = 0;
        }
    }
    return 0;
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t RxData = (uint8_t)USART_ReceiveData(USART1);
        Serial_RxPush(RxData);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

