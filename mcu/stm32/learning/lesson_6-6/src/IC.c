/**
 * @file    IC.c
 * @brief   Input Capture for Frequency Measurement
 * @note    Uses TIM3 Channel 1 on PA6
 */

#include "stm32f10x.h"

/**
 * @brief  Initialize input capture on TIM3 CH1 (PA6)
 */
void IC_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   // 使能TIM3时钟（APB1总线）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟（APB2总线）

    /* ========== 配置PA6为输入捕获引脚 ========== */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;          // 上拉输入模式：悬空时为高电平
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;              // PA6引脚（TIM3_CH1固定映射到PA6）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 50MHz速度：保证快速响应信号边沿
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 // 应用配置到GPIOA

    TIM_InternalClockConfig(TIM3);                         // 配置TIM3使用内部时钟源（72MHz）

    /* ========== 配置定时器时基单元 ========== */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    // 时钟分割：不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;               // ARR = 65535（16位最大值）
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               // PSC = 71（72MHz ÷ 72 = 1MHz）
    // 计数器频率：1MHz → 每个计数=1微秒
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;             // 重复计数器：0（普通模式不用）
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             // 应用时基配置到TIM3

    /* ========== 配置输入捕获通道1 ========== */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                // 选择通道1（PA6）
    TIM_ICInitStructure.TIM_ICFilter = 0xF;                          // 数字滤波器：0xF（最大滤波，15个时钟周期）
    // 滤波作用：消除输入信号的毛刺和噪声，提高测量精度
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     // 上升沿触发捕获（0→1时刻）
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           // 捕获预分频：不分频（每次上升沿都捕获）
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直连输入：TI1直接映射到IC1
    TIM_ICInit(TIM3, &TIM_ICInitStructure);                          // 应用输入捕获配置到TIM3

    /* ========== 配置从模式：自动复位 ========== */
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);           // 选择触发源：TI1FP1（通道1滤波后的信号）
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);        // 从模式：复位模式
    // 工作原理：每次检测到上升沿时，定时器计数器CNT自动复位为0，同时将当前CNT值存入CCR1
    // 这样CCR1中存储的就是两次上升沿之间的计数值（即一个周期的计数值）

    TIM_Cmd(TIM3, ENABLE);                                  // 启动TIM3定时器
}

/**
 * @brief  Get measured frequency
 * @return Frequency in Hz
 */
uint32_t IC_GetFreq(void)
{
    // 频率计算公式：Freq = 计数器频率 ÷ 周期计数值
    // 计数器频率 = 1MHz（72MHz ÷ 72）
    // 周期计数值 = CCR1（两次上升沿之间的计数值）
    // Freq = 1,000,000 ÷ (CCR1 + 1)
    // 
    // 示例：
    // 如果测量1kHz信号：
    //   周期 = 1ms = 1000微秒
    //   CCR1 = 1000 - 1 = 999（因为复位模式会在第1000个计数时复位）
    //   Freq = 1,000,000 ÷ (999 + 1) = 1000 Hz
    return 1000000 / (TIM_GetCapture1(TIM3) + 1);          // 读取CCR1捕获值并计算频率
}
