/**
 * @file    main_timer.c
 * @brief   Lesson 6-1: Timer Interrupt Demo
 * @note    Demonstrates TIM2 periodic interrupt with OLED display
 *
 * Hardware Connections:
 *   OLED Display (4-pin I2C):
 *     - SCL -> PB8
 *     - SDA -> PB9
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 * Expected Behavior:
 *   - OLED shows "Lesson 6-1" and "Timer Interrupt"
 *   - Num value updates every second in timer interrupt
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"

int main(void)
{
    /* Initialize modules */
    OLED_Init();
    Timer_Init();

    /* Display static text */
    OLED_ShowString(1, 1, "Lesson 6-1");
    OLED_ShowString(2, 1, "Timer Interrupt");
    OLED_ShowString(3, 1, "----------------");
    OLED_ShowString(4, 1, "Num:");

    /* Main loop */
    while (1)
    {
        /* Continuously refresh display with current value */
        OLED_ShowNum(4, 5, Num, 5);
    }
}
