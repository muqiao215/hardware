#include "stm32f10x.h"
#include "Motor.h"
#include "PWM_TIM2.h"

static void Motor_SetOne(int16_t speed, GPIO_TypeDef *dir_port, uint16_t in1, uint16_t in2, uint8_t pwm_ch)
{
    int16_t s = speed;
    if (s > 1000)
        s = 1000;
    if (s < -1000)
        s = -1000;

    if (s == 0)
    {
        GPIO_ResetBits(dir_port, in1);
        GPIO_ResetBits(dir_port, in2);
        PWM_TIM2_SetCompare(pwm_ch, 0);
        return;
    }

    if (s > 0)
    {
        GPIO_SetBits(dir_port, in1);
        GPIO_ResetBits(dir_port, in2);
    }
    else
    {
        GPIO_ResetBits(dir_port, in1);
        GPIO_SetBits(dir_port, in2);
        s = (int16_t)-s;
    }

    uint16_t period = PWM_TIM2_GetPeriod();
    uint16_t compare = (uint16_t)((uint32_t)s * (period + 1) / 1000);
    if (compare > period)
        compare = period;
    PWM_TIM2_SetCompare(pwm_ch, compare);
}

void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* TIM2 PWM @ 20kHz (72MHz / 3600 = 20kHz) */
    PWM_TIM2_Init(0, 3600 - 1);
    PWM_TIM2_EnableChannel(1);
    PWM_TIM2_EnableChannel(2);

    Motor_Stop();
}

void Motor_Set(int16_t left, int16_t right)
{
    Motor_SetOne(left, GPIOB, GPIO_Pin_0, GPIO_Pin_1, 1);
    Motor_SetOne(right, GPIOB, GPIO_Pin_12, GPIO_Pin_13, 2);
}

void Motor_Stop(void)
{
    Motor_Set(0, 0);
}

