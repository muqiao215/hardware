#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Delay.h"
#include "Encoder_TIM.h"
#include "Motor.h"
#include "OLED.h"
#include "Serial.h"
#include "Watchdog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int16_t s_cmd_left = 0;
static int16_t s_cmd_right = 0;
static uint32_t s_cmd_tick = 0;

static void set_cmd(int16_t l, int16_t r)
{
    s_cmd_left = l;
    s_cmd_right = r;
    s_cmd_tick = 0;
}

static void cmd_process(char *line)
{
    while (*line == ' ')
        line++;
    if (*line == '@')
        line++;

    if (*line == '\0')
        return;

    if (line[0] == 'S' || strncmp(line, "stop", 4) == 0)
    {
        set_cmd(0, 0);
        Serial_SendString("OK stop\r\n");
        return;
    }

    if (line[0] == 'F')
    {
        int v = atoi(line + 1);
        set_cmd(v, v);
        Serial_SendString("OK fwd\r\n");
        return;
    }

    if (line[0] == 'B')
    {
        int v = atoi(line + 1);
        set_cmd(-v, -v);
        Serial_SendString("OK back\r\n");
        return;
    }

    if (line[0] == 'L')
    {
        int v = atoi(line + 1);
        set_cmd(v / 2, v);
        Serial_SendString("OK left\r\n");
        return;
    }

    if (line[0] == 'R')
    {
        int v = atoi(line + 1);
        set_cmd(v, v / 2);
        Serial_SendString("OK right\r\n");
        return;
    }

    if (line[0] == 'V')
    {
        char *p = line + 1;
        int l = (int)strtol(p, &p, 10);
        int r = (int)strtol(p, &p, 10);
        set_cmd((int16_t)l, (int16_t)r);
        Serial_SendString("OK v\r\n");
        return;
    }

    Serial_SendString("ERR cmd\r\n");
}

int main(void)
{
    OLED_Init();
    Serial_Init(9600);
    Watchdog_InitMs(2000);

    Motor_Init();
    Encoder_TIM3_Init();
    Encoder_TIM4_Init();

    uint8_t adc_ch[2] = {2, 3};
    ADC_DMA_Init(adc_ch, 2);

    OLED_Clear();
    OLED_ShowString(1, 1, "BT Car 9600");
    OLED_ShowString(4, 1, "Cmd:F/B/L/R/V/S");

    Serial_SendString("\r\n[bluetooth_car] ready.\r\n");
    Serial_SendString("Cmd examples:\r\n");
    Serial_SendString("  F 600\r\n  B 600\r\n  L 600\r\n  R 600\r\n  V 500 -500\r\n  S\r\n");

    uint32_t loop_ms = 0;
    while (1)
    {
        Watchdog_Feed();

        char line[128];
        if (Serial_ReadLine(line, sizeof(line)))
        {
            cmd_process(line);
            s_cmd_tick = 0;
        }

        /* Failsafe: stop if no cmd for 1000ms */
        if (s_cmd_tick >= 1000)
        {
            Motor_Stop();
        }
        else
        {
            Motor_Set(s_cmd_left, s_cmd_right);
        }

        if ((loop_ms % 100) == 0)
        {
            int16_t e1 = Encoder_TIM3_GetDelta();
            int16_t e2 = Encoder_TIM4_GetDelta();
            char l2[17], l3[17];
            snprintf(l2, sizeof(l2), "L:%4d R:%4d", (int)s_cmd_left, (int)s_cmd_right);
            snprintf(l3, sizeof(l3), "E:%5d %5d", (int)e1, (int)e2);
            OLED_ShowString(2, 1, l2);
            OLED_ShowString(3, 1, l3);
        }

        Delay_ms(10);
        loop_ms += 10;
        s_cmd_tick += 10;
    }
}
