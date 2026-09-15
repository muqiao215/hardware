/**
 * @file    Key.c
 * @brief   Simple Key Input Detection
 * @note    Uses PB1 as key input (active low)
 */

#include "stm32f10x.h"
#include "Delay.h"

/**
 * @brief  Initialize key input pins (PB1, PB2)
 */
void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能GPIOB时钟，用于按键输入

    /* ========== 配置PB1, PB2为上拉输入模式 ========== */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // 上拉输入模式：未按下时为高电平（1）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;  // PB1(调速), PB2(方向切换)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 50MHz速度：快速响应按键变化
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 // 应用配置到GPIOB
    // 按键电路：PB1/PB2通过按键连接到GND，按下时为低电平（0），未按下时为高电平（1）
}

/**
 * @brief  Get key press
 * @return Key number (1 for PB1 pressed)
 * @note   Includes debounce delay
 */
uint8_t Key_GetNum(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)     // 检测PB1是否为低电平（按键按下）
    {
        Delay_ms(20);                                      // 延时20ms：消抖处理，等待按键稳定
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0); // 等待按键释放（PB1变为高电平）
        Delay_ms(20);                                      // 再次延时20ms：释放消抖，确保按键完全释放
        return 1;                                          // 返回按键编号1（表示按键1被按下并释放）
    }
    return 0;                                              // 按键未按下，返回0
    // 消抖原理：机械按键按下/释放时会有抖动，通过延时过滤掉抖动信号，只检测稳定的按键状态
}
