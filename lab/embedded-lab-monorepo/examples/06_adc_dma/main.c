#include "board.h"
#include "delay.h"

static volatile uint16_t g_adc_samples[2];

static void adc_dma_init(void) {
    GPIO_InitTypeDef gpio = {0};
    ADC_InitTypeDef adc = {0};
    DMA_InitTypeDef dma = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOA, &gpio);

    dma.DMA_BufferSize = 2;
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_M2M = DMA_M2M_Disable;
    dma.DMA_MemoryBaseAddr = (uint32_t)g_adc_samples;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Channel1, &dma);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    adc.ADC_ContinuousConvMode = ENABLE;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_NbrOfChannel = 2;
    adc.ADC_ScanConvMode = ENABLE;
    ADC_Init(ADC1, &adc);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET) {
    }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET) {
    }

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

int main(void) {
    board_led_init();
    adc_dma_init();

    while (1) {
        if (g_adc_samples[0] > g_adc_samples[1]) {
            board_led_on();
        } else {
            board_led_off();
        }
        delay_ms(50);
    }
}
