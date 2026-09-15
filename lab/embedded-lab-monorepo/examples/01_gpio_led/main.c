#include "board.h"
#include "delay.h"

int main(void) {
    board_led_init();

    while (1) {
        board_led_toggle();
        delay_ms(250);
    }
}
