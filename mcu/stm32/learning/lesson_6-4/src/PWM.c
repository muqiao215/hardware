/**
 * @file    PWM.c
 * @brief   PWM Initialization (Servo + LED)
 * @note    Servo: TIM2_CH2 @ PA1 (50Hz). LED: TIM2_CH1 @ PA0 (50Hz).
 *
 * PWM Configuration (50Hz, 20ms period):
 *   - TIM2 running at 72MHz
 *   - Prescaler: 72 -> 1MHz counter clock
 *   - Period: 20000 -> 50Hz PWM frequency (20ms period)
 *   - Servo CCR2 range: 500-2500 for 0-180 degrees (0.5ms~2.5ms)
 *   - LED CCR1 range: 0~20000 for 0~100%
 */

#include "stm32f10x.h"

/**
 * @brief  Initialize PWM on TIM2 (CH1 @ PA0, CH2 @ PA1)
 */
void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; /* PA0=CH1, PA1=CH2 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    /* Servo default to 90deg (1500us pulse) */
    TIM_OCInitStructure.TIM_Pulse = 1500;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  Set PWM CCR1 (LED)
 * @param  Compare: 0~20000
 */
void PWM_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare1(TIM2, Compare);
}

/**
 * @brief  Set PWM CCR2 (Servo pulse width in us)
 * @param  Compare: 0~20000
 */
void PWM_SetCompare2(uint16_t Compare)
{
    TIM_SetCompare2(TIM2, Compare);
}
