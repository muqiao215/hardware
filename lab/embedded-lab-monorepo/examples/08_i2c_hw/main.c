#include "board.h"
#include "delay.h"
#include "i2c_bus.h"

int main(void) {
    board_led_init();
    i2c_bus_init(100000, 0x30);

    while (1) {
        if (i2c_bus_probe(0x3C)) {
            board_led_on();
        } else {
            board_led_toggle();
        }
        delay_ms(500);
    }
}
