#include "stm32f10x.h"
#include "PWM_TIM2.h"

static uint16_t s_period = 0;

void PWM_TIM2_Init(uint16_t prescaler, uint16_t period)
{
    s_period = period;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = period;
    TIM_TimeBaseInitStructure.TIM_Prescaler = prescaler;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_ARRPreloadConfig(TIM2, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
}

void PWM_TIM2_EnableChannel(uint8_t channel)
{
    uint16_t pin = 0;
    if (channel == 1)
        pin = GPIO_Pin_0;
    else if (channel == 2)
        pin = GPIO_Pin_1;
    else if (channel == 3)
        pin = GPIO_Pin_2;
    else if (channel == 4)
        pin = GPIO_Pin_3;
    else
        return;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;

    if (channel == 1)
    {
        TIM_OC1Init(TIM2, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }
    else if (channel == 2)
    {
        TIM_OC2Init(TIM2, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }
    else if (channel == 3)
    {
        TIM_OC3Init(TIM2, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }
    else if (channel == 4)
    {
        TIM_OC4Init(TIM2, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }
}

void PWM_TIM2_SetCompare(uint8_t channel, uint16_t compare)
{
    if (compare > s_period)
    {
        compare = s_period;
    }

    if (channel == 1)
        TIM_SetCompare1(TIM2, compare);
    else if (channel == 2)
        TIM_SetCompare2(TIM2, compare);
    else if (channel == 3)
        TIM_SetCompare3(TIM2, compare);
    else if (channel == 4)
        TIM_SetCompare4(TIM2, compare);
}

uint16_t PWM_TIM2_GetPeriod(void)
{
    return s_period;
}

