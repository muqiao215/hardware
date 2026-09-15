/**
 * @file    main_servo.c
 * @brief   Lesson 6-4: Servo Sweeping + Speed Gears + LED Sync
 * @note    Servo uses TIM2_CH2 on PA1 (50Hz). LED uses TIM2_CH1 on PA0 (50Hz).
 *
 * Hardware Connections:
 *   Servo Motor (SG90):
 *     - Signal -> PA1
 *     - VCC -> 5V
 *     - GND -> GND
 *
 *   LED (with resistor):
 *     - Anode(+) -> PA0
 *     - Cathode(-) -> GND
 *
 *   OLED Display (4-pin I2C):
 *     - SCL -> PB8
 *     - SDA -> PB9
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 *   Key Pin:
 *     - Key1 -> PB1
 *
 * Expected Behavior:
 *   - Servo continuously sweeps 0~180~0
 *   - Key1 cycles speed gears
 *   - LED brightness pulses and scales with current speed gear
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Key.h"
#include "PWM.h"

typedef struct
{
    uint8_t step_deg;
    uint16_t delay_ms;
    uint8_t speed_percent;
} ServoGear;

static const ServoGear g_gears[] =
{
    {1, 20, 20},   /* slow */
    {2, 15, 40},
    {3, 10, 60},
    {5, 6, 80},
    {8, 4, 100},   /* fast */
};

static uint16_t PercentToCcr(uint8_t percent)
{
    if (percent > 100)
    {
        percent = 100;
    }
    return (uint16_t)((uint32_t)percent * 20000 / 100);
}

int main(void)
{
    OLED_Init();
    Servo_Init();
    Key_Init();

    float angle = 0;
    int8_t dir = 1;
    uint8_t gear = 0;

    OLED_ShowString(1, 1, "A:000 G:0 S:000");
    OLED_ShowString(2, 1, "LED:000%       ");
    OLED_ShowString(3, 1, "0<----^----->180");
    OLED_ShowString(4, 1, "Key1: Gear+     ");

    while (1)
    {
        if (Key_GetNum() == 1)
        {
            gear++;
            if (gear >= (sizeof(g_gears) / sizeof(g_gears[0])))
            {
                gear = 0;
            }
        }

        angle += (float)(dir * g_gears[gear].step_deg);
        if (angle >= 180)
        {
            angle = 180;
            dir = -1;
        }
        else if (angle <= 0)
        {
            angle = 0;
            dir = 1;
        }

        Servo_SetAngle(angle);

        /* triangle wave: 0..100..0 (sync with sweep) */
        uint8_t wave = (angle <= 90) ? (uint8_t)(angle * 100 / 90) : (uint8_t)((180 - angle) * 100 / 90);
        uint8_t led_percent = (uint8_t)((uint32_t)wave * g_gears[gear].speed_percent / 100);
        PWM_SetCompare1(PercentToCcr(led_percent));

        OLED_ShowNum(1, 3, (uint16_t)angle, 3);
        OLED_ShowNum(1, 9, gear + 1, 1);
        OLED_ShowNum(1, 14, g_gears[gear].speed_percent, 3);
        OLED_ShowNum(2, 5, led_percent, 3);

        Delay_ms(g_gears[gear].delay_ms);
    }
}
