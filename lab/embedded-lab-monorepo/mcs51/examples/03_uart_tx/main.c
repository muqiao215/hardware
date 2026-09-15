#include "uart0.h"

static void delay_loop(void) {
    volatile unsigned int outer;
    volatile unsigned int inner;

    for (outer = 0; outer < 600U; outer++) {
        for (inner = 0; inner < 200U; inner++) {
        }
    }
}

void main(void) {
    uart0_init_4800_11m0592();

    while (1) {
        uart0_send_cstr("uart ok\r\n");
        delay_loop();
    }
}
