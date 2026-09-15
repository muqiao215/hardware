#include "stm32f10x.h"
#include "Buzzer.h"
#include "Delay.h"
#include "Key.h"
#include "MyRTC.h"
#include "OLED.h"
#include "Store.h"

#include <stdio.h>

typedef enum
{
    POMO_IDLE = 0,
    POMO_WORK,
    POMO_BREAK,
    POMO_PAUSED,
} PomoState;

static uint16_t s_work_min = 25;
static uint16_t s_break_min = 5;
static PomoState s_state = POMO_IDLE;
static uint32_t s_end_ts = 0;
static uint32_t s_pause_left = 0;
static PomoState s_pause_from = POMO_WORK;

static void load_settings(void)
{
    if (Store_Data[10] >= 5 && Store_Data[10] <= 120)
        s_work_min = Store_Data[10];
    if (Store_Data[11] >= 1 && Store_Data[11] <= 60)
        s_break_min = Store_Data[11];
}

static void save_settings(void)
{
    Store_Data[10] = s_work_min;
    Store_Data[11] = s_break_min;
    Store_Save();
}

static void ui_draw(uint32_t now)
{
    char line[17];
    MyRTC_ReadTime();
    snprintf(line, sizeof(line), "%02d:%02d:%02d %3s", MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5], MyRTC_IsUsingLSE() ? "LSE" : "LSI");
    OLED_ShowString(1, 1, line);

    const char mode = (s_state == POMO_WORK) ? 'W' : (s_state == POMO_BREAK) ? 'B'
                               : (s_state == POMO_PAUSED) ? 'P'
                                                           : 'I';
    snprintf(line, sizeof(line), "%c W:%02u B:%02u        ", mode, s_work_min, s_break_min);
    OLED_ShowString(2, 1, line);

    uint32_t left = 0;
    if (s_state == POMO_WORK || s_state == POMO_BREAK)
    {
        left = (s_end_ts > now) ? (s_end_ts - now) : 0;
    }
    else if (s_state == POMO_PAUSED)
    {
        left = s_pause_left;
    }

    uint32_t mm = left / 60;
    uint32_t ss = left % 60;
    snprintf(line, sizeof(line), "Left %02lu:%02lu", (unsigned long)mm, (unsigned long)ss);
    OLED_ShowString(3, 1, line);

    OLED_ShowString(4, 1, "K1 start/pause  ");
}

static void start_work(uint32_t now)
{
    s_state = POMO_WORK;
    s_end_ts = now + s_work_min * 60;
}

static void start_break(uint32_t now)
{
    s_state = POMO_BREAK;
    s_end_ts = now + s_break_min * 60;
}

int main(void)
{
    OLED_Init();
    Key_Init();
    Buzzer_Init();
    Store_Init();
    MyRTC_Init();
    load_settings();

    OLED_Clear();
    OLED_ShowString(1, 1, "Pomodoro");
    Delay_ms(300);

    uint32_t last_ui = 0;
    while (1)
    {
        uint32_t now = RTC_GetCounter();

        uint8_t key = Key_GetNum();
        if (key == 1)
        {
            if (s_state == POMO_IDLE)
            {
                start_work(now);
            }
            else if (s_state == POMO_WORK || s_state == POMO_BREAK)
            {
                s_pause_left = (s_end_ts > now) ? (s_end_ts - now) : 0;
                s_pause_from = s_state;
                s_state = POMO_PAUSED;
            }
            else if (s_state == POMO_PAUSED)
            {
                /* resume */
                s_end_ts = now + s_pause_left;
                s_state = s_pause_from;
            }
        }
        else if (key == 2)
        {
            if (s_state == POMO_IDLE)
            {
                /* quick presets */
                if (s_work_min == 25)
                {
                    s_work_min = 50;
                    s_break_min = 10;
                }
                else if (s_work_min == 50)
                {
                    s_work_min = 15;
                    s_break_min = 3;
                }
                else
                {
                    s_work_min = 25;
                    s_break_min = 5;
                }
                save_settings();
                Buzzer_BeepMs(50);
            }
            else
            {
                /* reset */
                s_state = POMO_IDLE;
                s_end_ts = 0;
                s_pause_left = 0;
                Buzzer_BeepMs(30);
            }
        }

        if ((s_state == POMO_WORK || s_state == POMO_BREAK) && now >= s_end_ts)
        {
            Buzzer_BeepMs(120);
            Delay_ms(80);
            Buzzer_BeepMs(120);

            if (s_state == POMO_WORK)
            {
                start_break(now);
            }
            else
            {
                s_state = POMO_IDLE;
                s_end_ts = 0;
            }
        }

        if (now != last_ui)
        {
            last_ui = now;
            ui_draw(now);
        }

        __asm__("wfi");
    }
}
