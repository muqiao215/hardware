#ifndef TIM2_PWM_H
#define TIM2_PWM_H

#include <stdint.h>

void tim2_pwm_init(uint16_t period, uint16_t prescaler);
void tim2_pwm_set_duty(uint16_t compare);
void tim2_pwm_set_period(uint16_t period);

#endif
