#include "stm32f10x.h"
#include "ADC_DMA.h"

static volatile uint16_t s_adc_values[4];
static uint8_t s_adc_count = 0;

static uint16_t adc_channel_to_gpio_pin(uint8_t adc_channel)
{
    /* ADC1 channels 0..7 are PA0..PA7 */
    if (adc_channel <= 7)
    {
        return (uint16_t)(1u << adc_channel);
    }
    return 0;
}

void ADC_DMA_Init(const uint8_t *channels, uint8_t count)
{
    if (count == 0)
        return;
    if (count > 4)
        count = 4;
    s_adc_count = count;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6); /* 12MHz */

    /* GPIO analog for selected channels (PA0..PA7) */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    uint16_t pins = 0;
    for (uint8_t i = 0; i < count; i++)
    {
        pins |= adc_channel_to_gpio_pin(channels[i]);
        ADC_RegularChannelConfig(ADC1, channels[i], (uint8_t)(i + 1), ADC_SampleTime_55Cycles5);
    }
    GPIO_InitStructure.GPIO_Pin = pins;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_NbrOfChannel = count;
    ADC_Init(ADC1, &ADC_InitStructure);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)s_adc_values;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = count;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET)
    {
    }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET)
    {
    }

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint8_t ADC_DMA_GetCount(void)
{
    return s_adc_count;
}

const volatile uint16_t *ADC_DMA_GetValues(void)
{
    return s_adc_values;
}
