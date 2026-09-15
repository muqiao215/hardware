#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include <stdint.h>

void Watchdog_InitMs(uint32_t timeout_ms);
void Watchdog_Feed(void);

#endif

