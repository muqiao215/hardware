/**
 * @file    Servo.c
 * @brief   Servo Motor Control
 * @note    Wraps PWM for servo angle control
 *
 * Servo Configuration (SG90):
 *   - Period: 20ms (50Hz)
 *   - Pulse: 0.5ms (0°) to 2.5ms (180°)
 *   - CCR2 range: 500 to 2500 (TIM2 @ 1MHz counter clock)
 */

#include "stm32f10x.h"
#include "PWM.h"

/**
 * @brief  Initialize servo (just initializes PWM)
 */
void Servo_Init(void)
{
    PWM_Init();
}

/**
 * @brief  Set servo angle
 * @param  Angle: 0-180 degrees
 */
void Servo_SetAngle(float Angle)
{
    PWM_SetCompare2((uint16_t)(Angle / 180 * 2000 + 500));
}
