#include "bsp_delay.h"

void bsp_delay_short(void) {
    volatile uint32_t count = 48;

    while (count--) {
        __asm__("nop");
    }
}

void bsp_delay_ms(uint32_t ms) {
    while (ms--) {
        volatile uint32_t count = 7200;

        while (count--) {
            __asm__("nop");
        }
    }
}
