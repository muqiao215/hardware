#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Delay.h"
#include "MyRTC.h"
#include "OLED.h"
#include "Serial.h"
#include "Store.h"
#include "Watchdog.h"

#include <stdio.h>
#include <string.h>

static uint32_t s_last_sec = 0;

static void ui_draw(const volatile uint16_t *adc, uint8_t n)
{
    char line[17];

    MyRTC_ReadTime();
    snprintf(line, sizeof(line), "%02d:%02d:%02d %3s", MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5], MyRTC_IsUsingLSE() ? "LSE" : "LSI");
    OLED_ShowString(1, 1, line);

    if (n >= 2)
    {
        snprintf(line, sizeof(line), "A0:%4u A1:%4u", (unsigned)adc[0], (unsigned)adc[1]);
        OLED_ShowString(2, 1, line);
    }
    if (n >= 4)
    {
        snprintf(line, sizeof(line), "A2:%4u A3:%4u", (unsigned)adc[2], (unsigned)adc[3]);
        OLED_ShowString(3, 1, line);
    }
    OLED_ShowString(4, 1, "cmd:@status/save ");
}

static void store_last(const volatile uint16_t *adc, uint8_t n)
{
    /* Store layout:
     *   [1] sample_counter low16
     *   [2] sample_counter high16
     *   [3..] adc values (up to 4)
     */
    uint32_t cnt = ((uint32_t)Store_Data[2] << 16) | Store_Data[1];
    cnt++;
    Store_Data[1] = (uint16_t)(cnt & 0xFFFF);
    Store_Data[2] = (uint16_t)(cnt >> 16);
    for (uint8_t i = 0; i < n && i < 4; i++)
    {
        Store_Data[3 + i] = adc[i];
    }
    Store_Save();
}

static void cmd_process(char *cmd)
{
    if (cmd[0] == '@')
    {
        cmd++;
    }

    if (strcmp(cmd, "status") == 0)
    {
        uint32_t cnt = ((uint32_t)Store_Data[2] << 16) | Store_Data[1];
        Serial_Printf("samples=%lu\r\n", (unsigned long)cnt);
        return;
    }
    if (strcmp(cmd, "save") == 0)
    {
        const volatile uint16_t *adc = ADC_DMA_GetValues();
        store_last(adc, ADC_DMA_GetCount());
        Serial_SendString("OK save\r\n");
        return;
    }
    if (strcmp(cmd, "clear") == 0)
    {
        Store_Clear();
        Serial_SendString("OK clear\r\n");
        return;
    }
    Serial_SendString("ERR cmd\r\n");
}

int main(void)
{
    OLED_Init();
    Serial_Init(9600);
    Store_Init();
    MyRTC_Init();
    Watchdog_InitMs(2000);

    uint8_t ch[4] = {0, 1, 2, 3};
    ADC_DMA_Init(ch, 4);

    OLED_Clear();
    Serial_SendString("\r\n[smart_home_gateway] ready. cmds: @status/@save/@clear\r\n");

    while (1)
    {
        Watchdog_Feed();

        uint32_t now = RTC_GetCounter();
        if (now != s_last_sec)
        {
            s_last_sec = now;

            const volatile uint16_t *adc = ADC_DMA_GetValues();
            uint8_t n = ADC_DMA_GetCount();

            ui_draw(adc, n);
            Serial_Printf("T=%lu A0=%u A1=%u A2=%u A3=%u\r\n",
                          (unsigned long)now,
                          (unsigned)n > 0 ? adc[0] : 0,
                          (unsigned)n > 1 ? adc[1] : 0,
                          (unsigned)n > 2 ? adc[2] : 0,
                          (unsigned)n > 3 ? adc[3] : 0);
        }

        char cmd[128];
        if (Serial_ReadLine(cmd, sizeof(cmd)))
        {
            cmd_process(cmd);
        }

        __asm__("wfi");
    }
}
