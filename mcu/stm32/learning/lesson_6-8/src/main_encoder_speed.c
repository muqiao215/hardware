/**
 * @file    main_encoder_speed.c
 * @brief   Lesson 6-8: Encoder Interface Speed Measurement
 * @note    Demonstrates TIM3 encoder interface mode
 *
 * Hardware Connections:
 *   Rotary Encoder (with A/B phase):
 *     - A Phase -> PA6 (TIM3 CH1)
 *     - B Phase -> PA7 (TIM3 CH2)
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 *   OLED Display (4-pin I2C):
 *     - SCL -> PB8
 *     - SDA -> PB9
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 * Expected Behavior:
 *   - OLED shows "Speed: xxxxx"
 *   - Rotating encoder updates speed value
 *   - Speed is measured as encoder count per second
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"

int16_t Speed = 0;

int main(void)
{
    OLED_Init();
    Timer_Init();
    Encoder_Init();

    OLED_ShowString(1, 1, "Speed:");

    while (1)
    {
        OLED_ShowSignedNum(1, 7, Speed, 5);
    }
}

/**
 * @brief TIM2 Interrupt Handler (100ms interval)
 */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Speed = Encoder_Get();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
