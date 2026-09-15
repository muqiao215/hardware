#include "stc89c52_min.h"

static void delay_loop(void) {
    volatile unsigned int outer;
    volatile unsigned int inner;

    for (outer = 0; outer < 400U; outer++) {
        for (inner = 0; inner < 200U; inner++) {
        }
    }
}

void main(void) {
    while (1) {
        STC89C52_P20 = !STC89C52_P20;
        delay_loop();
    }
}
