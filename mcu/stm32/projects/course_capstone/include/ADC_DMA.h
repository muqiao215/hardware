#ifndef __ADC_DMA_H
#define __ADC_DMA_H

#include <stdint.h>

void ADC_DMA_Init(const uint8_t *channels, uint8_t count);
uint8_t ADC_DMA_GetCount(void);
const volatile uint16_t *ADC_DMA_GetValues(void);

#endif
