#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Delay.h"
#include "MyRTC.h"
#include "OLED.h"
#include "Watchdog.h"

#include <stdio.h>

static volatile uint8_t s_alarm = 0;

static void rtc_alarm_config(uint32_t seconds_from_now)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask();

    uint32_t now = RTC_GetCounter();
    RTC_SetAlarm(now + seconds_from_now);
    RTC_WaitForLastTask();

    s_alarm = 0;
}

void RTCAlarm_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);
        EXTI_ClearITPendingBit(EXTI_Line17);
        s_alarm = 1;
        RTC_WaitForLastTask();
    }
}

int main(void)
{
    OLED_Init();
    MyRTC_Init();
    Watchdog_InitMs(4000);

    uint8_t adc_ch[2] = {0, 1};
    ADC_DMA_Init(adc_ch, 2);

    OLED_Clear();
    OLED_ShowString(1, 1, "LowPower Sentry");
    OLED_ShowString(2, 1, "RTC ALRM wake");
    OLED_ShowString(4, 1, "STOP every 5s");

    uint32_t wake_cnt = 0;

    while (1)
    {
        Watchdog_Feed();

        const volatile uint16_t *adc = ADC_DMA_GetValues();
        char line[17];
        snprintf(line, sizeof(line), "Wake:%5lu", (unsigned long)wake_cnt);
        OLED_ShowString(3, 1, line);
        snprintf(line, sizeof(line), "A0:%4u A1:%4u", (unsigned)adc[0], (unsigned)adc[1]);
        OLED_ShowString(2, 1, line);

        rtc_alarm_config(5);

        PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
        SystemInit();

        if (s_alarm)
        {
            wake_cnt++;
        }
    }
}

