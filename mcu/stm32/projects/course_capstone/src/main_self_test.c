#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Buzzer.h"
#include "CountSensor.h"
#include "Delay.h"
#include "Encoder_TIM.h"
#include "Key.h"
#include "MPU6050.h"
#include "MyRTC.h"
#include "OLED.h"
#include "Serial.h"
#include "Store.h"
#include "W25Q64.h"

#include <stdio.h>
#include <string.h>

typedef enum
{
    T_OLED = 0,
    T_KEYS,
    T_BUZZER,
    T_ENCODER,
    T_MPU,
    T_ADC,
    T_RTC,
    T_W25Q,
    T_SENSOR,
    T_DONE,
} TestId;

static void header(TestId t)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "SELF TEST");
    OLED_ShowString(4, 1, "K1 next  K2 run");
    Serial_Printf("\r\n[self_test] select=%d\r\n", (int)t);
}

static void show_passfail(uint8_t pass)
{
    OLED_ShowString(3, 1, pass ? "PASS" : "FAIL");
    if (pass)
        Buzzer_BeepMs(20);
    else
        Buzzer_BeepMs(120);
}

static uint8_t test_oled(void)
{
    OLED_ShowString(2, 1, "OLED draw test");
    OLED_ShowString(3, 1, "0123456789ABCDEF");
    Delay_ms(300);
    OLED_ShowString(3, 1, "fedcba9876543210");
    Delay_ms(300);
    return 1;
}

static uint8_t test_keys(void)
{
    OLED_ShowString(2, 1, "Press K1/K2...");
    for (uint32_t i = 0; i < 200; i++)
    {
        uint8_t k = Key_GetNum();
        if (k)
        {
            OLED_ShowString(3, 1, "Key:");
            OLED_ShowNum(3, 5, k, 1);
            return 1;
        }
        Delay_ms(10);
    }
    return 0;
}

static uint8_t test_buzzer(void)
{
    OLED_ShowString(2, 1, "Buzzer beep...");
    Buzzer_BeepMs(80);
    Delay_ms(60);
    Buzzer_BeepMs(80);
    return 1;
}

static uint8_t test_encoder(void)
{
    OLED_ShowString(2, 1, "Enc: rotate...");
    int32_t sum1 = 0, sum2 = 0;
    for (uint32_t i = 0; i < 200; i++)
    {
        sum1 += Encoder_TIM3_GetDelta();
        sum2 += Encoder_TIM4_GetDelta();
        if ((i % 20) == 0)
        {
            char line[17];
            snprintf(line, sizeof(line), "E3:%6ld", (long)sum1);
            OLED_ShowString(3, 1, line);
            snprintf(line, sizeof(line), "E4:%6ld", (long)sum2);
            OLED_ShowString(3, 10, line + 3);
        }
        if ((sum1 > 10) || (sum1 < -10) || (sum2 > 10) || (sum2 < -10))
        {
            return 1;
        }
        Delay_ms(10);
    }
    return 0;
}

static uint8_t test_mpu(void)
{
    OLED_ShowString(2, 1, "MPU6050...");
    uint8_t id = MPU6050_GetID();
    OLED_ShowString(3, 1, "ID:0x");
    OLED_ShowHexNum(3, 6, id, 2);
    return (id != 0x00 && id != 0xFF);
}

static uint8_t test_adc(void)
{
    OLED_ShowString(2, 1, "ADC DMA...");
    const volatile uint16_t *v = ADC_DMA_GetValues();
    char line[17];
    snprintf(line, sizeof(line), "A0:%4u A1:%4u", (unsigned)v[0], (unsigned)v[1]);
    OLED_ShowString(3, 1, line);
    return 1;
}

static uint8_t test_rtc(void)
{
    OLED_ShowString(2, 1, "RTC init...");
    MyRTC_Init();
    MyRTC_ReadTime();
    char line[17];
    snprintf(line, sizeof(line), "%02d:%02d:%02d %3s", MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5], MyRTC_IsUsingLSE() ? "LSE" : "LSI");
    OLED_ShowString(3, 1, line);
    return 1;
}

static uint8_t test_w25q(void)
{
    OLED_ShowString(2, 1, "W25Q64...");
    uint8_t mid;
    uint16_t did;
    W25Q64_ReadID(&mid, &did);
    OLED_ShowString(3, 1, "MID:");
    OLED_ShowHexNum(3, 5, mid, 2);
    OLED_ShowString(3, 8, "DID:");
    OLED_ShowHexNum(3, 12, did, 4);

    if (mid == 0xFF || mid == 0x00)
        return 0;

    uint8_t w[16];
    uint8_t r[16];
    for (int i = 0; i < 16; i++)
        w[i] = (uint8_t)(0xA5 ^ i);

    W25Q64_SectorErase(0x000000);
    W25Q64_PageProgram(0x000000, w, 16);
    memset(r, 0, sizeof(r));
    W25Q64_ReadData(0x000000, r, 16);
    return (memcmp(w, r, 16) == 0);
}

static uint8_t test_sensor(void)
{
    OLED_ShowString(2, 1, "PB14 count...");
    CountSensor_Reset();
    for (uint32_t i = 0; i < 300; i++)
    {
        uint16_t c = CountSensor_Get();
        OLED_ShowString(3, 1, "Count:");
        OLED_ShowNum(3, 8, c, 5);
        if (c > 0)
            return 1;
        Delay_ms(10);
    }
    return 0;
}

int main(void)
{
    OLED_Init();
    Serial_Init(9600);
    Key_Init();
    Buzzer_Init();
    Store_Init();

    Encoder_TIM3_Init();
    Encoder_TIM4_Init();

    MPU6050_Init();
    W25Q64_Init();

    uint8_t ch[2] = {0, 1};
    ADC_DMA_Init(ch, 2);

    CountSensor_Init();

    TestId t = T_OLED;
    header(t);

    while (1)
    {
        uint8_t k = Key_GetNum();
        if (k == 1)
        {
            t = (TestId)((t + 1) % T_DONE);
            header(t);
        }
        else if (k == 2)
        {
            uint8_t pass = 0;
            if (t == T_OLED)
                pass = test_oled();
            else if (t == T_KEYS)
                pass = test_keys();
            else if (t == T_BUZZER)
                pass = test_buzzer();
            else if (t == T_ENCODER)
                pass = test_encoder();
            else if (t == T_MPU)
                pass = test_mpu();
            else if (t == T_ADC)
                pass = test_adc();
            else if (t == T_RTC)
                pass = test_rtc();
            else if (t == T_W25Q)
                pass = test_w25q();
            else if (t == T_SENSOR)
                pass = test_sensor();
            else
                pass = 1;

            show_passfail(pass);
        }

        Delay_ms(10);
    }
}

