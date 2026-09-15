/**
 * @file    main_encoder.c
 * @brief   Lesson 5-2: Rotary Encoder Counter with OLED Display
 * @note    Demonstrates quadrature encoder decoding via EXTI interrupts
 *
 * Hardware Connections:
 *   Rotary Encoder:
 *     - CLK (A) -> PB0
 *     - DT  (B) -> PB1
 *     - SW      -> Not used
 *     - VCC     -> 3.3V
 *     - GND     -> GND
 *
 *   OLED Display (4-pin I2C):
 *     - SCL -> PB8
 *     - SDA -> PB9
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 * Expected Behavior:
 *   - OLED shows "Num: xxxxx"
 *   - Rotate encoder clockwise: number increases
 *   - Rotate encoder counter-clockwise: number decreases
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Encoder.h"

int main(void)
{
    /* Variable to accumulate encoder value */
    int16_t Num = 0;

    /* Initialize modules */
    OLED_Init();
    Encoder_Init();

    /* Display static label */
    OLED_ShowString(1, 1, "Lesson 5-2");
    OLED_ShowString(2, 1, "Rotary Encoder");
    OLED_ShowString(3, 1, "----------------");
    OLED_ShowString(4, 1, "Num:");

    /* Main loop */
    while (1)
    {
        /*
         * Encoder_Get() returns the increment since last call,
         * then resets its internal counter to 0.
         * This "delta" approach allows smooth accumulation.
         */
        Num += Encoder_Get();

        /* Display current value (signed, 5 digits) */
        OLED_ShowSignedNum(4, 5, Num, 5);
    }
}
