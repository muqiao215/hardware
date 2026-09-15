#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Delay.h"
#include "Encoder_TIM.h"
#include "Key.h"
#include "Motor.h"
#include "MPU6050.h"
#include "OLED.h"
#include "PID.h"

#include <math.h>
#include <stdio.h>

static float rad2deg(float r)
{
    return r * 57.2957795f;
}

int main(void)
{
    OLED_Init();
    Key_Init();

    MPU6050_Init();
    uint8_t id = MPU6050_GetID();

    Encoder_TIM3_Init();
    Encoder_TIM4_Init();
    Motor_Init();

    uint8_t adc_ch[2] = {2, 3}; /* PA2/PA3 */
    ADC_DMA_Init(adc_ch, 2);

    PID_t pid;
    PID_Init(&pid, 22.0f, 0.0f, 0.8f, -1000.0f, 1000.0f);

    float angle = 0.0f;
    uint8_t enabled = 0;

    OLED_Clear();
    OLED_ShowString(1, 1, "BalanceBall MPU");
    OLED_ShowString(2, 1, "ID:0x");
    OLED_ShowHexNum(2, 6, id, 2);
    OLED_ShowString(4, 1, "K1=run/stop");

    while (1)
    {
        uint8_t k = Key_GetNum();
        if (k == 1)
        {
            enabled = !enabled;
            PID_Reset(&pid);
            Motor_Stop();
        }

        int16_t ax, ay, az, gx, gy, gz;
        MPU6050_GetData(&ax, &ay, &az, &gx, &gy, &gz);

        /* Convert to approximate units */
        float acc_y = (float)ay / 2048.0f;
        float acc_z = (float)az / 2048.0f;
        float gyro_x_dps = (float)gx / 16.4f;

        float acc_angle = rad2deg(atan2f(acc_y, acc_z));

        const float dt = 0.01f;
        angle = 0.98f * (angle + gyro_x_dps * dt) + 0.02f * acc_angle;

        int16_t enc_l = Encoder_TIM3_GetDelta();
        int16_t enc_r = Encoder_TIM4_GetDelta();

        float error = 0.0f - angle;
        float out = PID_Update(&pid, error, dt);

        /* Speed damping */
        float speed = (float)(enc_l + enc_r);
        out -= speed * 0.5f;

        if (enabled)
        {
            Motor_Set((int16_t)out, (int16_t)out);
        }
        else
        {
            Motor_Stop();
        }

        char line[17];
        snprintf(line, sizeof(line), "Ang:%5.1f %c     ", angle, enabled ? 'R' : 'S');
        OLED_ShowString(2, 1, line);
        snprintf(line, sizeof(line), "Out:%4d E:%5d", (int)out, (int)(enc_l + enc_r));
        OLED_ShowString(3, 1, line);

        Delay_ms(10);
    }
}
