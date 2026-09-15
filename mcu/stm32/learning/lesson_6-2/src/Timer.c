/**
 * @file    Timer.c
 * @brief   Lesson 6-2: Timer External Clock Mode
 * @note    Configures TIM2 with external clock from PA0 (ETR pin)
 *
 * External Clock Configuration:
 *   - TIM2_ETR -> PA0 (fixed mapping)
 *   - Prescaler: 1 (no division)
 *   - Period: 10
 *   - ETR polarity: Non-inverted
 *   - Filter: 0x0F (maximum)
 */

#include "stm32f10x.h"

/**
 * @brief  Initialize TIM2 with external clock mode
 */
void Timer_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIOA0 as input (ETR) */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* External clock mode 2 */
    TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF,
                           TIM_ExtTRGPolarity_NonInverted, 0x0F);

    /* Time base unit */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  Get current counter value
 * @return Counter value (0-65535)
 */
uint16_t Timer_GetCounter(void)
{
    return TIM_GetCounter(TIM2);
}