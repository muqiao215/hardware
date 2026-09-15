#include "stm32f10x.h"
#include "oled.h"

static void frame_delay(uint32_t loops) {
    while (loops--) {
        __asm__("nop");
    }
}

int main(void) {
    oled_init();

    while (1) {
        oled_draw_smiley();
        oled_update();
        frame_delay(4500000);

        oled_draw_cryface(0);
        oled_update();
        frame_delay(2500000);

        oled_draw_cryface(4);
        oled_update();
        frame_delay(2500000);
    }
}
