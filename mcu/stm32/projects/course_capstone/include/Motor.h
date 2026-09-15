#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

/* Dual-motor L298N-style driver (fixed pin map for this capstone):
 * - PWM: TIM2 CH1 (PA0) -> Left EN
 * - PWM: TIM2 CH2 (PA1) -> Right EN
 * - Left DIR:  PB0/PB1  (IN1/IN2)
 * - Right DIR: PB12/PB13 (IN3/IN4)
 */

void Motor_Init(void);
void Motor_Set(int16_t left, int16_t right);
void Motor_Stop(void);

#endif

