#ifndef __PWM_TIM2_H
#define __PWM_TIM2_H

#include <stdint.h>

void PWM_TIM2_Init(uint16_t prescaler, uint16_t period);
void PWM_TIM2_EnableChannel(uint8_t channel);
void PWM_TIM2_SetCompare(uint8_t channel, uint16_t compare);
uint16_t PWM_TIM2_GetPeriod(void);

#endif

