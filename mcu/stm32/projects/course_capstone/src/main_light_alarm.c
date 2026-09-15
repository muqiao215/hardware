#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Delay.h"
#include "OLED.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BUZZER_GPIO GPIOA
#define BUZZER_PIN GPIO_Pin_0
#define BUTTON_GPIO GPIOA
#define BUTTON_PIN GPIO_Pin_1

#define LIGHT_ADC_CHANNEL 2
#define LIGHT_DARK_THRESHOLD 1800u

static void show_line(uint8_t line, const char *fmt, ...)
{
    char text[17];
    char tmp[32];
    va_list args;

    memset(text, ' ', 16);
    text[16] = '\0';

    va_start(args, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);

    for (uint8_t i = 0; i < 16 && tmp[i] != '\0'; i++)
    {
        text[i] = tmp[i];
    }

    OLED_ShowString(line, 1, text);
}

static void io_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);
}

static void buzzer_on(void)
{
    GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);
}

static void buzzer_off(void)
{
    GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);
}

static uint8_t button_pressed_event(void)
{
    if (GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN) == 0)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN) == 0)
        {
            while (GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN) == 0)
            {
            }
            Delay_ms(20);
            return 1;
        }
    }

    return 0;
}

int main(void)
{
    uint8_t adc_channel = LIGHT_ADC_CHANNEL;
    uint8_t alarm_enabled = 1;

    io_init();
    OLED_Init();
    ADC_DMA_Init(&adc_channel, 1);

    OLED_Clear();
    show_line(1, "Light Alarm");
    show_line(4, "PA1 toggles EN");

    while (1)
    {
        const volatile uint16_t *adc = ADC_DMA_GetValues();
        uint16_t light = adc[0];
        uint8_t percent = (uint8_t)(((uint32_t)light * 100u) / 4095u);
        uint8_t dark = (light < LIGHT_DARK_THRESHOLD);

        if (button_pressed_event())
        {
            alarm_enabled = !alarm_enabled;
            buzzer_off();
        }

        show_line(2, "A2:%4u %3u%%", (unsigned)light, (unsigned)percent);
        show_line(3, "TH:%4u %s %s",
                  (unsigned)LIGHT_DARK_THRESHOLD,
                  alarm_enabled ? "EN" : "OFF",
                  dark ? "DARK" : "OK");

        if (alarm_enabled && dark)
        {
            buzzer_on();
            Delay_ms(60);
            buzzer_off();
            Delay_ms(140);
        }
        else
        {
            buzzer_off();
            Delay_ms(100);
        }
    }
}
