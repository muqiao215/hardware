#include "stm32f10x.h"
#include "Encoder_TIM.h"

static void Encoder_InitTIM(TIM_TypeDef *tim, uint32_t rcc_tim, GPIO_TypeDef *gpio, uint32_t rcc_gpio, uint16_t pin1, uint16_t pin2)
{
    if (tim == TIM3)
        RCC_APB1PeriphClockCmd(rcc_tim, ENABLE);
    else
        RCC_APB1PeriphClockCmd(rcc_tim, ENABLE);

    RCC_APB2PeriphClockCmd(rcc_gpio, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = pin1 | pin2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(gpio, &GPIO_InitStructure);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(tim, &TIM_TimeBaseInitStructure);

    TIM_EncoderInterfaceConfig(tim, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_SetCounter(tim, 0);
    TIM_Cmd(tim, ENABLE);
}

void Encoder_TIM3_Init(void)
{
    Encoder_InitTIM(TIM3, RCC_APB1Periph_TIM3, GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_6, GPIO_Pin_7);
}

void Encoder_TIM4_Init(void)
{
    Encoder_InitTIM(TIM4, RCC_APB1Periph_TIM4, GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_6, GPIO_Pin_7);
}

static int16_t Encoder_GetDelta(TIM_TypeDef *tim)
{
    int16_t delta = (int16_t)TIM_GetCounter(tim);
    TIM_SetCounter(tim, 0);
    return delta;
}

int16_t Encoder_TIM3_GetDelta(void)
{
    return Encoder_GetDelta(TIM3);
}

int16_t Encoder_TIM4_GetDelta(void)
{
    return Encoder_GetDelta(TIM4);
}

