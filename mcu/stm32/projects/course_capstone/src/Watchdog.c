#include "stm32f10x.h"
#include "Watchdog.h"

void Watchdog_InitMs(uint32_t timeout_ms)
{
    /* IWDG clocked by LSI (~40kHz). Use prescaler 32 for a decent range. */
    const uint32_t lsi_hz = 40000;
    const uint32_t prescaler = 32;
    uint32_t reload = (timeout_ms * (lsi_hz / 1000)) / prescaler;
    if (reload == 0)
    {
        reload = 1;
    }
    if (reload > 0x0FFF)
    {
        reload = 0x0FFF;
    }

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    IWDG_SetReload((uint16_t)(reload - 1));
    IWDG_ReloadCounter();
    IWDG_Enable();
}

void Watchdog_Feed(void)
{
    IWDG_ReloadCounter();
}

