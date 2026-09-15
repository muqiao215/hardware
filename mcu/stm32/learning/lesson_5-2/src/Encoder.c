/**
 * @file    Encoder.c
 * @brief   Rotary Encoder Driver using EXTI interrupts
 * @note    Lesson 5-2: Demonstrates quadrature decoding via software
 *
 * Hardware Connection:
 *   - Encoder A (CLK) -> PB0 (EXTI0)
 *   - Encoder B (DT)  -> PB1 (EXTI1)
 *   - Encoder C (SW)  -> Not used (button)
 *   - VCC -> 3.3V
 *   - GND -> GND
 *
 * Quadrature Encoding Principle:
 *   Clockwise:      A leads B by 90 degrees
 *   Counter-CW:     B leads A by 90 degrees
 *
 *   Direction detection:
 *   - On A falling edge: if B is LOW -> CCW, if B is HIGH -> CW
 *   - On B falling edge: if A is LOW -> CW,  if A is HIGH -> CCW
 */

#include "stm32f10x.h"

/* Global counter for encoder increments */
int16_t Encoder_Count = 0;

/**
 * @brief  Initialize rotary encoder GPIOs and EXTI interrupts
 */
void Encoder_Init(void)
{
    /* ========== 1. Enable Clocks ========== */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // Required for EXTI

    /* ========== 2. GPIO Configuration ========== */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // Input with pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* ========== 3. AFIO: Map GPIO to EXTI Lines ========== */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);  // PB0 -> EXTI0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);  // PB1 -> EXTI1

    /* ========== 4. EXTI Configuration ========== */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // Falling edge
    EXTI_Init(&EXTI_InitStructure);

    /* ========== 5. NVIC Priority Group (call once per project) ========== */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // 2-bit preemption, 2-bit sub

    /* ========== 6. NVIC Configuration for EXTI0 ========== */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    /* ========== 7. NVIC Configuration for EXTI1 ========== */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  // Slightly lower priority
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  Get encoder increment and reset counter
 * @return Accumulated increment since last call
 */
int16_t Encoder_Get(void)
{
    int16_t temp = Encoder_Count;
    Encoder_Count = 0;
    return temp;
}

/* ===========================================================================
 * INTERRUPT SERVICE ROUTINES
 * ===========================================================================
 * ISR names MUST match startup_stm32f10x_md.s exactly!
 * ===========================================================================
 */

/**
 * @brief  EXTI0 interrupt handler (A phase - PB0)
 *
 * TODO [Learning Exercise]: Implement direction detection logic
 *
 * When A phase (PB0) has a falling edge:
 *   - Read B phase (PB1) level
 *   - If B is LOW:  Counter-clockwise -> Encoder_Count--
 *   - If B is HIGH: Clockwise         -> Encoder_Count++
 *
 * Don't forget to:
 *   1. Check if this interrupt was actually triggered (EXTI_GetITStatus)
 *   2. Optionally re-read PB0 to filter noise
 *   3. Clear the interrupt pending bit (EXTI_ClearITPendingBit)
 */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET)
    {
        /* Noise filter: confirm PB0 is actually LOW */
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
        {
            /*
             * Direction detection: A falling edge, check B phase
             * - B is LOW  -> Counter-clockwise (CCW) -> decrement
             * - B is HIGH -> Clockwise (CW)          -> increment
             */
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
            {
                Encoder_Count--;  // CCW: B is already LOW when A falls
            }
            else
            {
                Encoder_Count++;  // CW: B is still HIGH when A falls
            }
        }

        /* CRITICAL: Clear interrupt flag to prevent infinite re-entry */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/**
 * @brief  EXTI1 interrupt handler (B phase - PB1)
 *
 * TODO [Learning Exercise]: Implement direction detection logic
 *
 * When B phase (PB1) has a falling edge:
 *   - Read A phase (PB0) level
 *   - If A is LOW:  Clockwise         -> Encoder_Count++
 *   - If A is HIGH: Counter-clockwise -> Encoder_Count--
 */
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET)
    {
        /* Noise filter: confirm PB1 is actually LOW */
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
        {
            /*
             * Direction detection: B falling edge, check A phase
             * - A is LOW  -> Clockwise (CW)          -> increment
             * - A is HIGH -> Counter-clockwise (CCW) -> decrement
             */
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
            {
                Encoder_Count++;  // CW: A is already LOW when B falls
            }
            else
            {
                Encoder_Count--;  // CCW: A is still HIGH when B falls
            }
        }

        /* CRITICAL: Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
