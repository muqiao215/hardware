#ifndef STC89C52RC_BOARD_H
#define STC89C52RC_BOARD_H

#include "stc89c52_min.h"

#define STC89C52_CLOCK_HZ 11059200UL
#define STC89C52_UART0_BAUD 4800U

#define STC89C52_BOARD_LED0 STC89C52_P20

/*
 * Default key mapping for the first debounce example.
 *
 * WARNING:
 * P3.1 is also the classic 8051 UART0 TXD pin. Keep this as a board-level
 * alias so it can be changed in one place once the real learning board wiring
 * is confirmed.
 */
#define STC89C52_BOARD_KEY0_RAW_PRESSED() (STC89C52_P31 == 0)

#endif
