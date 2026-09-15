#include "board.h"
#include "delay.h"

static void adc_init_single(void) {
    GPIO_InitTypeDef gpio = {0};
    ADC_InitTypeDef adc = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &gpio);

    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_NbrOfChannel = 1;
    adc.ADC_ScanConvMode = DISABLE;
    ADC_Init(ADC1, &adc);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET) {
    }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET) {
    }
}

static uint16_t adc_read_channel0(void) {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {
    }
    return ADC_GetConversionValue(ADC1);
}

int main(void) {
    board_led_init();
    adc_init_single();

    while (1) {
        uint16_t value = adc_read_channel0();
        if (value > 2048U) {
            board_led_on();
        } else {
            board_led_off();
        }
        delay_ms(50);
    }
}
