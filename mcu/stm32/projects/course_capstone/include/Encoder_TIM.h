#ifndef __ENCODER_TIM_H
#define __ENCODER_TIM_H

#include <stdint.h>

void Encoder_TIM3_Init(void); /* PA6/PA7 */
void Encoder_TIM4_Init(void); /* PB6/PB7 */

int16_t Encoder_TIM3_GetDelta(void);
int16_t Encoder_TIM4_GetDelta(void);

#endif

