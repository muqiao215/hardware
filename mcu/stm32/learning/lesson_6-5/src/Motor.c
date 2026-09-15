/**
 * @file    Motor.c
 * @brief   DC Motor Control with L298N Driver
 * @note    Uses TIM2 Channel 3 on PA2 for PWM speed control
 *
 * Hardware Connection:
 *   - ENA (PWM) -> PA2
 *   - IN1 -> PA4
 *   - IN2 -> PA5
 *   - LED_Fwd (Green) -> PA0 (正转指示)
 */

#include "stm32f10x.h"
#include "PWM.h"

/**
 * @brief  Initialize motor control pins
 */
void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟，用于方向控制引脚

    /* ========== 配置方向控制引脚（IN1和IN2）+ 正转LED ========== */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       // 推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_5;  // PA0=LED, PA4=IN1, PA5=IN2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz速度
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_0);              // PA0 初始低电平（LED熄灭）

    PWM_Init();                                        // 初始化PWM：配置PA2作为PWM输出（ENA使能）
}

/**
 * @brief  Set forward LED (PA0) state
 * @param  State: 1=ON, 0=OFF
 */
void Motor_SetLed(uint8_t State)
{
    if (State)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    }
}

/**
 * @brief  Toggle forward LED (PA0)
 */
void Motor_ToggleLed(void)
{
    static uint8_t state = 0;
    state = !state;
    if (state)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    }
}

/**
 * @brief  Set motor speed
 * @param  Speed: -100 to 100
 *           >0: forward
 *           <0: reverse
 */
void Motor_SetSpeed(int8_t Speed)
{
    if (Speed >= 0)                                        // 如果速度≥0，电机正转
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4);                  // IN1 = 1（高电平）
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);                 // IN2 = 0（低电平）
        // L298N真值表：IN1=1, IN2=0 → 电机正转
        PWM_SetCompare3(Speed);                            // 设置PWM占空比 = Speed（0-100%）
    }
    else                                                   // 如果速度<0，电机反转
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);                // IN1 = 0（低电平）
        GPIO_SetBits(GPIOA, GPIO_Pin_5);                   // IN2 = 1（高电平）
        // L298N真值表：IN1=0, IN2=1 → 电机反转
        PWM_SetCompare3(-Speed);                           // 设置PWM占空比 = -Speed（取绝对值，0-100%）
        // 注意：Speed是负数，-Speed变成正数作为占空比
    }
}
