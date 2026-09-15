#ifndef __ADC_AUDIO_H
#define __ADC_AUDIO_H

#include <stdint.h>

void ADC_Audio_Init(uint32_t sample_rate_hz);
int ADC_Audio_GetBlock(const uint16_t **block, uint16_t *count);

#endif

