/**
 * @file    Timer.h
 * @brief   Timer interrupt function declarations
 */

#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

extern uint16_t Num;  // Variable incremented in timer interrupt

/**
 * @brief  Initialize TIM2 for periodic interrupt
 * @note   Interrupt triggers every 1 second
 */
void Timer_Init(void);

#endif
