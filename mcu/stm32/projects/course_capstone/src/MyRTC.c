#include "stm32f10x.h"
#include "MyRTC.h"

#include <time.h>

uint16_t MyRTC_Time[] = {2023, 1, 1, 23, 59, 55};

static uint8_t s_using_lse = 0;

uint8_t MyRTC_IsUsingLSE(void)
{
    return s_using_lse;
}

static int wait_flag(FlagStatus (*getter)(uint8_t), uint8_t flag, uint32_t timeout)
{
    while (getter(flag) != SET)
    {
        if (timeout-- == 0)
            return 0;
    }
    return 1;
}

void MyRTC_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);

    PWR_BackupAccessCmd(ENABLE);

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        /* Prefer LSE (32.768k), fallback to LSI if not present */
        s_using_lse = 0;
        RCC_LSEConfig(RCC_LSE_ON);
        if (wait_flag(RCC_GetFlagStatus, RCC_FLAG_LSERDY, 2000000))
        {
            s_using_lse = 1;
            RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        }
        else
        {
            RCC_LSEConfig(RCC_LSE_OFF);
            RCC_LSICmd(ENABLE);
            (void)wait_flag(RCC_GetFlagStatus, RCC_FLAG_LSIRDY, 2000000);
            RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
        }

        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();

        if (s_using_lse)
        {
            RTC_SetPrescaler(32768 - 1);
        }
        else
        {
            /* LSI is imprecise; use nominal 40kHz */
            RTC_SetPrescaler(40000 - 1);
        }
        RTC_WaitForLastTask();

        MyRTC_SetTime();

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else
    {
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
    }
}

void MyRTC_SetTime(void)
{
    time_t time_cnt;
    struct tm time_date;

    time_date.tm_year = MyRTC_Time[0] - 1900;
    time_date.tm_mon = MyRTC_Time[1] - 1;
    time_date.tm_mday = MyRTC_Time[2];
    time_date.tm_hour = MyRTC_Time[3];
    time_date.tm_min = MyRTC_Time[4];
    time_date.tm_sec = MyRTC_Time[5];

    time_cnt = mktime(&time_date) - 8 * 60 * 60;

    RTC_SetCounter((uint32_t)time_cnt);
    RTC_WaitForLastTask();
}

void MyRTC_ReadTime(void)
{
    time_t time_cnt;
    struct tm time_date;

    time_cnt = (time_t)RTC_GetCounter() + 8 * 60 * 60;
    time_date = *localtime(&time_cnt);

    MyRTC_Time[0] = (uint16_t)(time_date.tm_year + 1900);
    MyRTC_Time[1] = (uint16_t)(time_date.tm_mon + 1);
    MyRTC_Time[2] = (uint16_t)(time_date.tm_mday);
    MyRTC_Time[3] = (uint16_t)(time_date.tm_hour);
    MyRTC_Time[4] = (uint16_t)(time_date.tm_min);
    MyRTC_Time[5] = (uint16_t)(time_date.tm_sec);
}

