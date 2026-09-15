#include "stm32f10x.h"
#include "Buzzer.h"
#include "CountSensor.h"
#include "Delay.h"
#include "Key.h"
#include "MyRTC.h"
#include "OLED.h"
#include "Store.h"

#include <stdio.h>

/* Daily counter using RTC seconds / 86400, stored in internal flash Store_Data.
 * Layout:
 *  - [30] base_day (low16), [31] base_day (high16)
 *  - [32..38] counts for day0..day6
 */

static uint32_t get_day(void)
{
    return RTC_GetCounter() / 86400;
}

static void rotate_days(uint32_t new_day)
{
    uint32_t base = ((uint32_t)Store_Data[31] << 16) | Store_Data[30];
    if (base == 0)
    {
        base = new_day;
        Store_Data[30] = (uint16_t)(base & 0xFFFF);
        Store_Data[31] = (uint16_t)(base >> 16);
        Store_Save();
        return;
    }

    if (new_day <= base)
        return;

    uint32_t diff = new_day - base;
    if (diff > 7)
        diff = 7;

    for (uint32_t i = 0; i < diff; i++)
    {
        for (int d = 0; d < 6; d++)
        {
            Store_Data[32 + d] = Store_Data[32 + d + 1];
        }
        Store_Data[38] = 0;
        base++;
    }

    Store_Data[30] = (uint16_t)(base & 0xFFFF);
    Store_Data[31] = (uint16_t)(base >> 16);
    Store_Save();
}

int main(void)
{
    OLED_Init();
    Key_Init();
    Buzzer_Init();
    Store_Init();
    MyRTC_Init();
    CountSensor_Init();

    OLED_Clear();
    OLED_ShowString(1, 1, "Habit Tracker");
    OLED_ShowString(4, 1, "PB14 events +K2");

    uint16_t last_sensor = 0;
    uint32_t last_day = get_day();
    rotate_days(last_day);

    while (1)
    {
        uint32_t day = get_day();
        if (day != last_day)
        {
            last_day = day;
            rotate_days(day);
            Buzzer_BeepMs(50);
        }

        uint16_t c = CountSensor_Get();
        if (c != last_sensor)
        {
            uint16_t diff = (uint16_t)(c - last_sensor);
            last_sensor = c;
            Store_Data[38] = (uint16_t)(Store_Data[38] + diff);
            Store_Save();
        }

        uint8_t k = Key_GetNum();
        if (k == 2)
        {
            Store_Data[38]++;
            Store_Save();
            Buzzer_BeepMs(20);
        }
        else if (k == 1)
        {
            for (int i = 0; i < 7; i++)
            {
                Store_Data[32 + i] = 0;
            }
            Store_Data[38] = 0;
            Store_Save();
            Buzzer_BeepMs(120);
        }

        char l2[17], l3[17];
        snprintf(l2, sizeof(l2), "Today:%5u", (unsigned)Store_Data[38]);
        snprintf(l3, sizeof(l3), "D-1:%4u D-2:%4u", (unsigned)Store_Data[37], (unsigned)Store_Data[36]);
        OLED_ShowString(2, 1, l2);
        OLED_ShowString(3, 1, l3);

        __asm__("wfi");
    }
}

