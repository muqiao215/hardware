#include "stm32f10x.h"
#include "ADC_Audio.h"
#include "Delay.h"
#include "OLED.h"
#include "PWM_TIM2.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

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

static void bars_to_string(char *out, uint8_t bars)
{
    for (uint8_t i = 0; i < 16; i++)
    {
        out[i] = (i < bars) ? '#' : ' ';
    }
    out[16] = '\0';
}

int main(void)
{
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(1, 1, "Music Light");
    OLED_ShowString(2, 1, "ADC(PA3)+DMA");

    /* PWM for RGB LED on PA0/PA1/PA2 (TIM2 CH1/2/3) */
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

        float p[8];
        for (int i = 0; i < 8; i++)
        {
            p[i] = goertzel_power(block, n, coeffs[i]);
            smooth[i] = 0.8f * smooth[i] + 0.2f * p[i];
        }

        /* Map to simple 0..16 bar values */
        uint8_t bars[8];
        for (int i = 0; i < 8; i++)
        {
            float v = smooth[i] * 8.0f;
            if (v < 0)
                v = 0;
            if (v > 16)
                v = 16;
            bars[i] = (uint8_t)v;
        }

        char l3[17], l4[17];
        bars_to_string(l3, bars[0] > bars[1] ? bars[0] : bars[1]);
        bars_to_string(l4, bars[6] > bars[7] ? bars[6] : bars[7]);
        OLED_ShowString(3, 1, l3);
        OLED_ShowString(4, 1, l4);

        /* RGB: low/mid/high energy */
        float low = smooth[0] + smooth[1] + smooth[2];
        float mid = smooth[3] + smooth[4] + smooth[5];
        float high = smooth[6] + smooth[7];

        uint16_t period = PWM_TIM2_GetPeriod();
        uint16_t r = (uint16_t)fminf((low * 400.0f), (float)period);
        uint16_t g = (uint16_t)fminf((mid * 400.0f), (float)period);
        uint16_t b = (uint16_t)fminf((high * 600.0f), (float)period);

        PWM_TIM2_SetCompare(1, r);
        PWM_TIM2_SetCompare(2, g);
        PWM_TIM2_SetCompare(3, b);
    }
}

