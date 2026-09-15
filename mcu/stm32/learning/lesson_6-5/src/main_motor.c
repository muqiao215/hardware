/**
 * @file    main_motor.c
 * @brief   Lesson 6-5: PWM DC Motor Control with LED Direction Indicator
 * @note    Demonstrates DC motor speed control using TIM2 Channel 3 on PA2
 *
 * Hardware Connections:
 *   DC Motor Driver (L298N or similar):
 *     - ENA (PWM) -> PA2
 *     - IN1 -> PA4
 *     - IN2 -> PA5
 *     - GND -> GND
 *
 *   Direction LED:
 *     - Forward LED (Green) -> PA0
 *     - Connect: PA0 -> 220Ω -> LED(+) -> LED(-) -> GND
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
 *   - Press Key1 to cycle speed: 0 -> 20 -> 40 -> ... -> 100 -> -100
 *   - Positive values: forward rotation, PA0 LED pulses at speed
 *   - Negative values: reverse rotation, PA0 LED off
 *   - OLED shows current speed and direction arrow
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"

uint8_t KeyNum = 0;
int8_t Speed = 0;
uint16_t LedCounter = 0;  // LED 炫酷闪烁计数器
uint32_t ReinitCounter = 0;  // OLED 重初始化计数器

// 获取速度的绝对值
static uint8_t GetSpeedAbs(void)
{
    return (Speed >= 0) ? Speed : -Speed;
}

// 更新 OLED 显示
static void UpdateOLED(void)
{
    uint8_t speedAbs = GetSpeedAbs();

    // 第1行: 速度值
    OLED_ShowString(1, 1, "Speed:");
    OLED_ShowSignedNum(1, 7, Speed, 4);

    // 第2行: 方向 + 进度条
    OLED_ShowString(2, 1, "Dir:");

    if (Speed > 0)
    {
        OLED_ShowString(2, 5, "FWD");  // Forward
        // 正转进度条（从左到右）
        uint8_t bars = speedAbs / 10;  // 0-10格
        OLED_ShowString(2, 9, "          ");  // 清空
        for (uint8_t i = 0; i < bars && i < 10; i++)
        {
            OLED_ShowChar(2, 9 + i, '>');
        }
    }
    else if (Speed < 0)
    {
        OLED_ShowString(2, 5, "REV");  // Reverse
        // 反转进度条（从右到左）
        uint8_t bars = speedAbs / 10;
        OLED_ShowString(2, 9, "          ");  // 清空
        for (uint8_t i = 0; i < bars && i < 10; i++)
        {
            OLED_ShowChar(2, 18 - i, '<');
        }
    }
    else
    {
        OLED_ShowString(2, 5, "STOP");
        OLED_ShowString(2, 10, "     ");
    }

    // 第3行: 百分比
    OLED_ShowString(3, 1, "PWM: ");
    OLED_ShowNum(3, 6, speedAbs, 3);
    OLED_ShowChar(3, 9, '%');

    // 第4行: 状态提示
    OLED_ShowString(4, 1, "Key1: Gear+   ");
}

// 炫酷 LED 效果（正转时根据速度闪烁）
static void UpdateCoolLED(void)
{
    if (Speed == 0)
    {
        Motor_SetLed(0);  // 停止时熄灭
        return;
    }

    uint8_t speedAbs = GetSpeedAbs();

    if (Speed > 0)  // 正转：PA0 炫酷闪烁
    {
        LedCounter++;
        // 闪烁频率随速度变化：速度越快，闪烁越快
        // Speed=20: 每10次切换, Speed=100: 每2次切换
        uint16_t toggleThreshold = 110 - speedAbs;

        if (LedCounter >= toggleThreshold)
        {
            Motor_ToggleLed();
            LedCounter = 0;
        }
    }
    else  // 反转：PA0 熄灭
    {
        Motor_SetLed(0);
    }
}

int main(void)
{
    OLED_Init();
    Motor_Init();
    Key_Init();

    // 初始显示
    OLED_Clear();
    UpdateOLED();

    while (1)
    {
        KeyNum = Key_GetNum();
        if (KeyNum == 1)
        {
            Speed += 20;
            if (Speed > 100)
            {
                Speed = -100;
            }
            // 切换后立即更新显示
            UpdateOLED();
        }

        Motor_SetSpeed(Speed);

        // 持续更新 LED 炫酷效果
        UpdateCoolLED();

        // 每 5 秒尝试重新初始化 OLED（解决供电切换后不显示问题）
        ReinitCounter++;
        if (ReinitCounter >= 500)  // 500 * 10ms = 5秒
        {
            OLED_Init();        // 重新初始化
            UpdateOLED();       // 重新显示
            ReinitCounter = 0;
        }

        Delay_ms(10);  // 10ms 循环周期
    }
}
