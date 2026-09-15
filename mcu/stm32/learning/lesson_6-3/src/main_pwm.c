/**
 * @file    main_pwm.c
 * @brief   Lesson 6-3: PWM LED Breathing Light
 * @note    Demonstrates PWM output with TIM2 Channel 1 on PA0
 *
 * Hardware Connections:
 *   LED (with current limiting resistor):
 *     - LED+ -> PA0
 *     - LED- -> GND
 *
 *   OLED Display (4-pin I2C):
 *     - SCL -> PB8
 *     - SDA -> PB9
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 * Expected Behavior:
 *   - LED gradually brightens (0-100%)
 *   - LED gradually dims (100-0%)
 *   - Cycle repeats continuously
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"

uint8_t i = 0;

int main(void)
{
    OLED_Init();
    PWM_Init();

    while (1)
    {
        for (i = 0; i <= 100; i++)
        {
            PWM_SetCompare1(i);
            Delay_ms(10);
        }
        for (i = 0; i <= 100; i++)
        {
            PWM_SetCompare1(100 - i);
            Delay_ms(10);
        }
    }
}
