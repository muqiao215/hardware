#include "board.h"
#include "delay.h"
#include "spi_bus.h"

int main(void) {
    uint8_t rx;

    board_led_init();
    spi_bus_init();

    while (1) {
        spi_bus_select();
        rx = spi_bus_transfer(0xA5);
        spi_bus_deselect();

        if (rx == 0xA5) {
            board_led_on();
        } else {
            board_led_toggle();
        }
        delay_ms(200);
    }
}
