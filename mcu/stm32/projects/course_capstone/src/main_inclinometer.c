#include "stm32f10x.h"
#include "Delay.h"
#include "MPU6050.h"
#include "MyRTC.h"
#include "OLED.h"
#include "Serial.h"
#include "Store.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static float s_off_roll = 0.0f;
static float s_off_pitch = 0.0f;

static float rad2deg(float r)
{
    return r * 57.2957795f;
}

static void load_cal(void)
{
    /* Store in 0.01 deg */
    int16_t r = (int16_t)Store_Data[20];
    int16_t p = (int16_t)Store_Data[21];
    s_off_roll = r / 100.0f;
    s_off_pitch = p / 100.0f;
}

static void save_cal(void)
{
    Store_Data[20] = (uint16_t)((int16_t)(s_off_roll * 100));
    Store_Data[21] = (uint16_t)((int16_t)(s_off_pitch * 100));
    Store_Save();
}

static void cmd(char *line, float roll, float pitch)
{
    if (strcmp(line, "cal") == 0)
    {
        s_off_roll = roll;
        s_off_pitch = pitch;
        save_cal();
        Serial_SendString("OK cal saved\r\n");
        return;
    }
    if (strcmp(line, "zero") == 0)
    {
        s_off_roll = 0;
        s_off_pitch = 0;
        save_cal();
        Serial_SendString("OK zero\r\n");
        return;
    }
    Serial_SendString("cmd: cal | zero\r\n");
}

int main(void)
{
    OLED_Init();
    Serial_Init(9600);
    Store_Init();
    MyRTC_Init();
    load_cal();

    MPU6050_Init();

    OLED_Clear();
    OLED_ShowString(1, 1, "Inclinometer");
    OLED_ShowString(4, 1, "UART: cal/zero");

    float roll = 0.0f;
    float pitch = 0.0f;

    while (1)
    {
        int16_t ax, ay, az, gx, gy, gz;
        MPU6050_GetData(&ax, &ay, &az, &gx, &gy, &gz);

        float fax = (float)ax / 2048.0f;
        float fay = (float)ay / 2048.0f;
        float faz = (float)az / 2048.0f;

        roll = rad2deg(atan2f(fay, faz)) - s_off_roll;
        pitch = rad2deg(atan2f(-fax, sqrtf(fay * fay + faz * faz))) - s_off_pitch;

        char l2[17], l3[17];
        snprintf(l2, sizeof(l2), "R:%6.1f P:%6.1f", roll, pitch);
        snprintf(l3, sizeof(l3), "off %4d %4d", (int)(s_off_roll * 10), (int)(s_off_pitch * 10));
        OLED_ShowString(2, 1, l2);
        OLED_ShowString(3, 1, l3);

        char line[128];
        if (Serial_ReadLine(line, sizeof(line)))
        {
            cmd(line, roll, pitch);
        }

        Delay_ms(50);
    }
}

