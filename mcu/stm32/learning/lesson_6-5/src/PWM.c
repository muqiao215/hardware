/**
 * @file    PWM.c
 * @brief   PWM (TIM2_CH3 @ PA2) for DC motor speed control
 *
 * PWM Configuration (course annotated source):
 *   - TIM2 clock: 72MHz
 *   - Prescaler: 36-1  -> 2MHz counter clock
 *   - Period:    100-1 -> 20kHz PWM
 *   - CCR range: 0~100
 */

#include "stm32f10x.h"

void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   // 使能TIM2时钟（APB1总线）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟（APB2总线）

    /* ========== 配置PA2为PWM输出（TIM2通道3） ========== */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       // 复用推挽输出：PA2作为TIM2_CH3的PWM输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;             // PA2引脚（TIM2通道3固定映射到PA2）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz速度：保证PWM波形质量
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // 应用配置到GPIOA

    TIM_InternalClockConfig(TIM2);                        // 配置TIM2使用内部时钟源（72MHz系统时钟）

    /* ========== 配置定时器时基单元 ========== */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    // 时钟分割：不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                 // ARR = 100-1 = 99（PWM周期100个计数）
    TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;               // PSC = 36-1 = 35（72MHz ÷ 36 = 2MHz）
    // PWM频率计算：2MHz ÷ 100 = 20kHz（适合电机控制，避免可听噪声）
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;             // 重复计数器：0（普通模式不用）
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);             // 应用时基配置到TIM2

    /* ========== 配置PWM输出通道3 ========== */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);              // 初始化结构体为默认值（安全措施）
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;    // PWM模式1：CNT < CCR时输出高电平
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性：高电平有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出：允许PWM信号输出到引脚
    TIM_OCInitStructure.TIM_Pulse = 0;                    // 初始占空比：0（CCR初始值，电机初始停止）
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);              // 应用配置到TIM2通道3（PA2）

    TIM_Cmd(TIM2, ENABLE);                                 // 启动TIM2定时器，开始产生PWM波形
}

void PWM_SetCompare3(uint16_t Compare)
{
    TIM_SetCompare3(TIM2, Compare);                        // 设置TIM2通道3的比较值（CCR3）
    // Compare范围：0-100，对应占空比0%-100%
    // 例如：Compare=50 → 占空比=50% → 电机50%速度
}
