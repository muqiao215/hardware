#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Servo.h"
#include "Encoder_TIM.h"
#include "Store.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static float s_angles[3] = {90.0f, 90.0f, 90.0f};
static uint8_t s_selected = 0;

static void angles_load(void)
{
    if (Store_Data[1] <= 1800 && Store_Data[2] <= 1800 && Store_Data[3] <= 1800)
    {
        s_angles[0] = Store_Data[1] / 10.0f;
        s_angles[1] = Store_Data[2] / 10.0f;
        s_angles[2] = Store_Data[3] / 10.0f;
    }
}

static void angles_save(void)
{
    Store_Data[1] = (uint16_t)(s_angles[0] * 10);
    Store_Data[2] = (uint16_t)(s_angles[1] * 10);
    Store_Data[3] = (uint16_t)(s_angles[2] * 10);
    Store_Save();
}

static void clamp_angles(void)
{
    for (int i = 0; i < 3; i++)
    {
        if (s_angles[i] < 0)
            s_angles[i] = 0;
        if (s_angles[i] > 180)
            s_angles[i] = 180;
    }
}

static void ui_draw(void)
{
    char line[17];

    snprintf(line, sizeof(line), "ARM SEL:J%d      ", s_selected + 1);
    OLED_ShowString(1, 1, line);

    snprintf(line, sizeof(line), "%03d %03d %03d     ", (int)s_angles[0], (int)s_angles[1], (int)s_angles[2]);
    OLED_ShowString(2, 1, line);

    OLED_ShowString(3, 1, "set a b c (deg)");
    OLED_ShowString(4, 1, "save/load/status ");
}

static void apply_servos(void)
{
    Servo_SetAngle(1, s_angles[0]);
    Servo_SetAngle(2, s_angles[1]);
    Servo_SetAngle(3, s_angles[2]);
}

static void cmd_help(void)
{
    Serial_SendString("Commands:\r\n");
    Serial_SendString("  set a b c   (0..180)\r\n");
    Serial_SendString("  save        (store angles)\r\n");
    Serial_SendString("  load        (load angles)\r\n");
    Serial_SendString("  status\r\n");
}

static void cmd_process(char *line)
{
    while (*line == ' ')
        line++;
    if (*line == '\0')
        return;

    if (strncmp(line, "help", 4) == 0)
    {
        cmd_help();
        return;
    }
    if (strncmp(line, "save", 4) == 0)
    {
        angles_save();
        Serial_SendString("OK save\r\n");
        return;
    }
    if (strncmp(line, "load", 4) == 0)
    {
        angles_load();
        clamp_angles();
        apply_servos();
        Serial_SendString("OK load\r\n");
        return;
    }
    if (strncmp(line, "status", 6) == 0)
    {
        Serial_Printf("J1=%d J2=%d J3=%d sel=%d\r\n", (int)s_angles[0], (int)s_angles[1], (int)s_angles[2], s_selected + 1);
        return;
    }
    if (strncmp(line, "set", 3) == 0)
    {
        char *p = line + 3;
        int a = (int)strtol(p, &p, 10);
        int b = (int)strtol(p, &p, 10);
        int c = (int)strtol(p, &p, 10);
        s_angles[0] = (float)a;
        s_angles[1] = (float)b;
        s_angles[2] = (float)c;
        clamp_angles();
        apply_servos();
        Serial_SendString("OK set\r\n");
        return;
    }

    Serial_SendString("ERR unknown, try 'help'\r\n");
}

int main(void)
{
    OLED_Init();
    Serial_Init(9600);
    Store_Init();
    angles_load();

    Servo_Init();
    Encoder_TIM3_Init();
    Encoder_TIM4_Init();

    clamp_angles();
    apply_servos();
    OLED_Clear();
    ui_draw();

    Serial_SendString("\r\n[gesture_arm] ready. type 'help' + Enter\r\n");

    uint32_t ui_tick = 0;
    while (1)
    {
        /* Encoder TIM3: selection, TIM4: angle */
        int16_t d_sel = Encoder_TIM3_GetDelta();
        int16_t d_ang = Encoder_TIM4_GetDelta();

        if (d_sel != 0)
        {
            int32_t step = d_sel / 4;
            if (step == 0)
                step = (d_sel > 0) ? 1 : -1;
            int32_t ns = (int32_t)s_selected + step;
            while (ns < 0)
                ns += 3;
            while (ns >= 3)
                ns -= 3;
            s_selected = (uint8_t)ns;
        }

        if (d_ang != 0)
        {
            float step = (float)(d_ang / 4);
            if (step == 0)
                step = (d_ang > 0) ? 1.0f : -1.0f;
            s_angles[s_selected] += step;
            clamp_angles();
            apply_servos();
        }

        char cmd[128];
        if (Serial_ReadLine(cmd, sizeof(cmd)))
        {
            cmd_process(cmd);
        }

        if ((ui_tick++ % 50) == 0)
        {
            ui_draw();
        }

        Delay_ms(10);
    }
}
