#include <stdint.h>

/* Minimal bare-metal main.
 * Replace with your BSP/HAL init and application code.
 */
int main(void) {
    // TODO: init clocks/peripherals here
    while (1) {
        __asm__("nop");
    }
}
