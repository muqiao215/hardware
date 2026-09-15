#include "Servo.h"
#include "PWM_TIM2.h"

static uint16_t Servo_ClampPulse(uint16_t us)
{
    if (us < 500)
        return 500;
    if (us > 2500)
        return 2500;
    return us;
}

void Servo_Init(void)
{
    /* TIM2: 72MHz / 72 = 1MHz tick => 1us per tick, 20ms period => 20000 ticks */
    PWM_TIM2_Init(72 - 1, 20000 - 1);
    PWM_TIM2_EnableChannel(1);
    PWM_TIM2_EnableChannel(2);
    PWM_TIM2_EnableChannel(3);
}

void Servo_SetAngle(uint8_t channel, float angle_deg)
{
    if (angle_deg < 0)
        angle_deg = 0;
    if (angle_deg > 180)
        angle_deg = 180;

    /* 0deg=500us, 180deg=2500us */
    uint16_t pulse = (uint16_t)(500.0f + (angle_deg / 180.0f) * 2000.0f);
    pulse = Servo_ClampPulse(pulse);
    PWM_TIM2_SetCompare(channel, pulse);
}

