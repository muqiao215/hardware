#ifndef STC89C52_UART0_H
#define STC89C52_UART0_H

#include <stdint.h>

void uart0_init_4800_11m0592(void);
void uart0_send_byte(uint8_t byte);
void uart0_send_cstr(const char *text);

#endif
