/**
 * @file    IC.c (PWMI version for lesson_6-7)
 * @brief   Input Capture with PWMI mode for Frequency & Duty
 */

#include "stm32f10x.h"

/**
 * @brief  Initialize input capture in PWMI mode
 */
void IC_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   // 使能TIM3时钟（APB1总线）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟（APB2总线）

    /* ========== 配置PA6为PWMI输入引脚 ========== */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;          // 上拉输入模式：悬空时为高电平
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;              // PA6引脚（TIM3_CH1固定映射到PA6）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 50MHz速度：保证快速响应信号边沿
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 // 应用配置到GPIOA
    // 注意：PWMI模式只需配置一个引脚（PA6），硬件会自动使用通道1和通道2

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

    /* ========== 配置PWMI模式（关键！） ========== */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                // 选择通道1作为主通道（PA6）
    TIM_ICInitStructure.TIM_ICFilter = 0xF;                          // 数字滤波器：0xF（最大滤波，15个时钟周期）
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     // 通道1：上升沿触发捕获
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           // 捕获预分频：不分频
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直连输入：TI1直接映射到IC1
    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);                      // 配置PWMI模式
    // ★ PWMI模式关键：TIM_PWMIConfig()自动配置两个通道！
    // - 通道1（IC1）：上升沿触发，测量周期（频率）
    // - 通道2（IC2）：下降沿触发，测量高电平时间（占空比）
    // 两个通道共用同一个输入引脚（PA6/TI1），硬件自动配置为互补极性

    /* ========== 配置从模式：自动复位 ========== */
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);           // 选择触发源：TI1FP1（通道1滤波后的信号）
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);        // 从模式：复位模式
    // 工作原理：每次检测到上升沿时，定时器计数器CNT自动复位为0
    // - CCR1捕获上升沿时刻的CNT值（即一个完整周期的计数值）
    // - CCR2捕获下降沿时刻的CNT值（即高电平持续的计数值）

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
    // 示例：测量1kHz信号
    //   周期 = 1ms = 1000微秒
    //   CCR1 = 1000 - 1 = 999
    //   Freq = 1,000,000 ÷ (999 + 1) = 1000 Hz
    return 1000000 / (TIM_GetCapture1(TIM3) + 1);          // 读取CCR1并计算频率
}

/**
 * @brief  Get measured duty cycle
 * @return Duty cycle in percentage (0-100%)
 */
uint32_t IC_GetDuty(void)
{
    // 占空比计算公式：Duty = (高电平时间 ÷ 周期时间) × 100%
    // 高电平时间 = CCR2（上升沿到下降沿的计数值）
    // 周期时间 = CCR1（两次上升沿之间的计数值）
    // Duty = (CCR2 + 1) × 100 ÷ (CCR1 + 1)
    // 
    // 示例：测量50%占空比的信号
    //   CCR1 = 1000 - 1 = 999（周期）
    //   CCR2 = 500 - 1 = 499（高电平时间）
    //   Duty = (499 + 1) × 100 ÷ (999 + 1) = 50%
    return (TIM_GetCapture2(TIM3) + 1) * 100 / (TIM_GetCapture1(TIM3) + 1); // 读取CCR2和CCR1计算占空比
}
