#include "stm32f10x.h"
#include "Delay.h"
#include "Encoder_TIM.h"
#include "Motor.h"
#include "OLED.h"
#include "PID.h"
#include "Serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PID_t s_pid;
static int32_t s_target = 0; /* counts per 100ms */
static uint8_t s_run = 0;

static void cmd(char *line)
{
    if (strncmp(line, "start", 5) == 0)
    {
        s_run = 1;
        PID_Reset(&s_pid);
        Serial_SendString("OK start\r\n");
        return;
    }
    if (strncmp(line, "stop", 4) == 0)
    {
        s_run = 0;
        Motor_Stop();
        Serial_SendString("OK stop\r\n");
        return;
    }
    if (strncmp(line, "t ", 2) == 0)
    {
        s_target = atoi(line + 2);
        Serial_SendString("OK t\r\n");
        return;
    }
    if (strncmp(line, "kp ", 3) == 0)
    {
        s_pid.kp = (float)atof(line + 3);
        Serial_SendString("OK kp\r\n");
        return;
    }
    if (strncmp(line, "ki ", 3) == 0)
    {
        s_pid.ki = (float)atof(line + 3);
        Serial_SendString("OK ki\r\n");
        return;
    }
    if (strncmp(line, "kd ", 3) == 0)
    {
        s_pid.kd = (float)atof(line + 3);
        Serial_SendString("OK kd\r\n");
        return;
    }

    Serial_SendString("cmd: start|stop|t <cnt>|kp <f>|ki <f>|kd <f>\r\n");
}

int main(void)
{
    OLED_Init();
    Serial_Init(9600);
    Motor_Init();
    Encoder_TIM3_Init();
    Encoder_TIM4_Init();

    PID_Init(&s_pid, 2.0f, 0.0f, 0.2f, -1000.0f, 1000.0f);

    OLED_Clear();
    OLED_ShowString(1, 1, "Motor PID Tuner");
    OLED_ShowString(4, 1, "UART cmd: start");
    Serial_SendString("\r\n[motor_pid_tuner] ready\r\n");

    int32_t meas = 0;
    int32_t out = 0;

    while (1)
    {
        char line[128];
        if (Serial_ReadLine(line, sizeof(line)))
        {
            cmd(line);
        }

        int16_t dl = Encoder_TIM3_GetDelta();
        int16_t dr = Encoder_TIM4_GetDelta();
        meas = (int32_t)(dl + dr);

        float err = (float)(s_target - meas);
        out = (int32_t)PID_Update(&s_pid, err, 0.1f);

        if (s_run)
        {
            Motor_Set((int16_t)out, (int16_t)out);
        }
        else
        {
            Motor_Stop();
        }

        char l2[17], l3[17];
        snprintf(l2, sizeof(l2), "T:%5ld M:%5ld", (long)s_target, (long)meas);
        snprintf(l3, sizeof(l3), "Out:%6ld %c", (long)out, s_run ? 'R' : 'S');
        OLED_ShowString(2, 1, l2);
        OLED_ShowString(3, 1, l3);

        Delay_ms(100);
    }
}

