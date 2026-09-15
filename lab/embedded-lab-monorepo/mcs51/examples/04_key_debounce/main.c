#include "debounce.h"
#include "board.h"
#include "uart0.h"

static void delay_loop(void) {
    volatile unsigned int outer;
    volatile unsigned int inner;

    for (outer = 0; outer < 20U; outer++) {
        for (inner = 0; inner < 200U; inner++) {
        }
    }
}

void main(void) {
    debounce_t key;

    uart0_init_4800_11m0592();
    debounce_init(&key, STC89C52_BOARD_KEY0_RAW_PRESSED() ? 1U : 0U, 3U);

    while (1) {
        debounce_update(&key, STC89C52_BOARD_KEY0_RAW_PRESSED() ? 1U : 0U);

        if (debounce_pressed_event(&key)) {
            STC89C52_BOARD_LED0 = !STC89C52_BOARD_LED0;
            uart0_send_cstr("key press\r\n");
        }

        if (debounce_released_event(&key)) {
            uart0_send_cstr("key release\r\n");
        }

        delay_loop();
    }
}
