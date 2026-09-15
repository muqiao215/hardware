/**
 * @file    main_ic.c
 * @brief   Lesson 6-6: Input Capture Frequency Measurement
 * @note    Demonstrates TIM3 input capture mode on PA6 (CH1)
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
 *   - PWM from TIM2 provides test signal
 *   - TIM3 measures input frequency
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
    IC_Init();             // 初始化TIM3输入捕获：PA6接收信号并测量频率

    OLED_ShowString(1, 1, "Freq:00000Hz");  // 在第1行显示"Freq:"标签

    /* ========== 配置测试信号：1kHz PWM ========== */
    // PWM频率计算：72MHz ÷ PSC ÷ ARR = 72MHz ÷ 720 ÷ 100 = 1kHz
    PWM_SetPrescaler(720 - 1);              // 设置预分频器：720-1（72MHz ÷ 720 = 100kHz计数时钟）
    PWM_SetCompare1(50);                    // 设置占空比：50%（CCR1 = 50）
    // 最终产生：1kHz, 50%占空比的PWM信号，输出到PA0

    /* ========== 主循环：持续读取并显示频率 ========== */
    while (1)
    {
        OLED_ShowNum(1, 6, IC_GetFreq(), 5);  // 在第1行第6列显示频率值（5位数字）
        // IC_GetFreq()通过输入捕获计算频率，返回值单位：Hz
        // 硬件连接：PA0（PWM输出）需要用杜邦线连接到PA6（输入捕获）
    }
}
