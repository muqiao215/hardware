#include "tim2_pwm.h"

#include "stm32f10x.h"

void tim2_pwm_init(uint16_t period, uint16_t prescaler) {
    GPIO_InitTypeDef gpio = {0};
    TIM_TimeBaseInitTypeDef base = {0};
    TIM_OCInitTypeDef oc = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    base.TIM_ClockDivision = TIM_CKD_DIV1;
    base.TIM_CounterMode = TIM_CounterMode_Up;
    base.TIM_Period = (uint16_t)(period - 1U);
    base.TIM_Prescaler = (uint16_t)(prescaler - 1U);
    base.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &base);

    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_Pulse = 0;
    TIM_OC1Init(TIM2, &oc);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void tim2_pwm_set_duty(uint16_t compare) {
    TIM_SetCompare1(TIM2, compare);
}

void tim2_pwm_set_period(uint16_t period) {
    TIM_SetAutoreload(TIM2, (uint16_t)(period - 1U));
}
