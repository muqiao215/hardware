#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void oled_init(void);
void oled_clear(void);
void oled_update(void);
void oled_draw_smiley(void);
void oled_draw_cryface(uint8_t tear_offset);

#endif
