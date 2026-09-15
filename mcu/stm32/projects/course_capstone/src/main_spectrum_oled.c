#include "stm32f10x.h"
#include "ADC_Audio.h"
#include "Delay.h"
#include "OLED.h"
#include "PWM_TIM2.h"

#include <math.h>
#include <stdio.h>

#define FS_HZ 8000

static float goertzel_power(const uint16_t *samples, uint16_t n, float coeff)
{
    float q0 = 0.0f, q1 = 0.0f, q2 = 0.0f;
    for (uint16_t i = 0; i < n; i++)
    {
        float x = ((float)samples[i] - 2048.0f) / 2048.0f;
        q0 = coeff * q1 - q2 + x;
        q2 = q1;
        q1 = q0;
    }
    return q1 * q1 + q2 * q2 - coeff * q1 * q2;
}

static char level_char(uint8_t level)
{
    if (level == 0)
        return ' ';
    if (level == 1)
        return '.';
    if (level == 2)
        return '*';
    return '#';
}

int main(void)
{
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(1, 1, "Spectrum OLED");
    OLED_ShowString(2, 1, "ADC PA3 DMA");

    PWM_TIM2_Init(72 - 1, 1000 - 1);
    PWM_TIM2_EnableChannel(1);
    PWM_TIM2_EnableChannel(2);
    PWM_TIM2_EnableChannel(3);

    ADC_Audio_Init(FS_HZ);

    const uint8_t ks[8] = {2, 4, 6, 9, 12, 16, 20, 24};
    float coeffs[8];
    for (int i = 0; i < 8; i++)
    {
        float w = 2.0f * 3.1415926f * (float)ks[i] / 128.0f;
        coeffs[i] = 2.0f * cosf(w);
    }

    float smooth[8] = {0};

    while (1)
    {
        const uint16_t *block;
        uint16_t n;
        if (!ADC_Audio_GetBlock(&block, &n))
        {
            __asm__("wfi");
            continue;
        }

        uint8_t lvl[8];
        for (int i = 0; i < 8; i++)
        {
            float p = goertzel_power(block, n, coeffs[i]);
            smooth[i] = 0.85f * smooth[i] + 0.15f * p;
            float v = smooth[i] * 12.0f;
            if (v < 0)
                v = 0;
            if (v > 3)
                v = 3;
            lvl[i] = (uint8_t)v;
        }

        char l3[17], l4[17];
        for (int i = 0; i < 8; i++)
        {
            l3[i] = level_char(lvl[i]);
            l4[i] = level_char((uint8_t)((lvl[i] + 1) % 4));
        }
        for (int i = 8; i < 16; i++)
        {
            l3[i] = ' ';
            l4[i] = ' ';
        }
        l3[16] = '\0';
        l4[16] = '\0';
        OLED_ShowString(3, 1, l3);
        OLED_ShowString(4, 1, l4);

        uint16_t period = PWM_TIM2_GetPeriod();
        PWM_TIM2_SetCompare(1, (uint16_t)((lvl[0] + lvl[1]) * (period / 8)));
        PWM_TIM2_SetCompare(2, (uint16_t)((lvl[3] + lvl[4]) * (period / 8)));
        PWM_TIM2_SetCompare(3, (uint16_t)((lvl[6] + lvl[7]) * (period / 6)));
    }
}

