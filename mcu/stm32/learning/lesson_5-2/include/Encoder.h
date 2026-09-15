#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

/**
 * @brief  Initialize rotary encoder on PB0 (A phase) and PB1 (B phase)
 *         Configures EXTI interrupts for both pins
 */
void Encoder_Init(void);

/**
 * @brief  Get encoder increment value since last call
 * @return Increment value (positive = clockwise, negative = counter-clockwise)
 * @note   This function clears the internal counter after reading
 */
int16_t Encoder_Get(void);

#endif
