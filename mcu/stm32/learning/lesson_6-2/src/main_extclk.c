/**
 * @file    main_extclk.c
 * @brief   Lesson 6-2: Timer External Clock with OLED Display
 * @note    Demonstrates TIM2 external clock mode with PA0 as ETR input
 *
 * Hardware Connections:
 *   External Clock Source (e.g., another MCU or signal generator):
 *     - ETR (TIM2) -> PA0
 *
 *   OLED Display (4-pin I2C):
 *     - SCL -> PB8
 *     - SDA -> PB9
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 * Expected Behavior:
 *   - OLED shows "Num: xxxxx" (timer overflow count)
 *   - OLED shows "CNT: xxxxx" (current counter value)
 *   - Counter increments on external clock pulses
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"

uint16_t Num = 0;  // Overflow counter

int main(void)
{
    OLED_Init();
    Timer_Init();

    OLED_ShowString(1, 1, "Num:");
    OLED_ShowString(2, 1, "CNT:");

    while (1)
    {
        OLED_ShowNum(1, 5, Num, 5);
        OLED_ShowNum(2, 5, Timer_GetCounter(), 5);
    }
}

/**
 * @brief TIM2 Interrupt Handler
 */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Num++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
