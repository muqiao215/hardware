/**
 * @file    main_pwmi.c
 * @brief   Lesson 6-7: PWMI Mode Frequency & Duty Cycle Measurement
 * @note    Demonstrates TIM3 PWMI mode on PA6 (CH1) and PA7 (CH2)
 *
 * Hardware Connections:
 *   PWM Signal Source (for testing):
 *     - PWM Output -> PA0 (from TIM2)
 *     - PWM Input (for capture) -> PA6 (to TIM3 CH1)
 *
 *   OLED Display (4-pin I2C):
 *     - SCL -> PB8
 *     - SDA -> PB9
 *     - VCC -> 3.3V
 *     - GND -> GND
 *
 * Expected Behavior:
 *   - OLED shows "Freq: xxxxx Hz"
 *   - OLED shows "Duty: xx%"
 *   - PWM from TIM2 provides test signal
 *   - TIM3 measures both frequency and duty cycle
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"

int main(void)
{
    OLED_Init();           // 初始化OLED显示屏（PB8=SCL, PB9=SDA）
    PWM_Init();            // 初始化TIM2 PWM输出：PA0作为测试信号源
    IC_Init();             // 初始化TIM3 PWMI模式：PA6同时测量频率和占空比

    OLED_ShowString(1, 1, "Freq:00000Hz");  // 在第1行显示"Freq:"标签
    OLED_ShowString(2, 1, "Duty:00%");      // 在第2行显示"Duty:"标签

    /* ========== 配置测试信号：1kHz, 50%占空比 ========== */
    // PWM频率计算：72MHz ÷ PSC ÷ ARR = 72MHz ÷ 720 ÷ 100 = 1kHz
    PWM_SetPrescaler(720 - 1);              // 设置预分频器：720-1（72MHz ÷ 720 = 100kHz）
    PWM_SetCompare1(50);                    // 设置占空比：50%（CCR1 = 50 / 100）
    // 最终产生：1kHz频率，50%占空比的PWM信号，输出到PA0

    /* ========== 主循环：持续读取并显示频率和占空比 ========== */
    while (1)
    {
        OLED_ShowNum(1, 6, IC_GetFreq(), 5);  // 在第1行第6列显示频率值（5位数字，单位：Hz）
        OLED_ShowNum(2, 6, IC_GetDuty(), 2);  // 在第2行第6列显示占空比（2位数字，单位：%）
        // 硬件连接：PA0（PWM输出）需要用杜邦线连接到PA6（PWMI输入）
        // PWMI模式特点：只需一个输入引脚（PA6），自动配置双通道同时测量频率和占空比
    }
}
