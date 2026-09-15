#include "board.h"
#include "stc89c52_min.h"
#include "uart0.h"

void uart0_init_4800_11m0592(void) {
    SCON = 0x40;
    PCON &= (uint8_t)~0x80U;

    TMOD &= 0x0FU;
    TMOD |= 0x20U;

    TL1 = 0xFAU;
    TH1 = 0xFAU;
    ET1 = 0;
    TR1 = 1;
}

void uart0_send_byte(uint8_t byte) {
    SBUF = byte;
    while (TI == 0) {
    }
    TI = 0;
}

void uart0_send_cstr(const char *text) {
    while (*text != '\0') {
        uart0_send_byte((uint8_t)*text);
        text++;
    }
}
