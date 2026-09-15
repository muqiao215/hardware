#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#include "framebuffer.h"

typedef void (*ssd1306_write_fn_t)(uint8_t address, const uint8_t *data, uint16_t length);

typedef struct {
    uint8_t address;
    ssd1306_write_fn_t write;
} ssd1306_t;

void ssd1306_init(ssd1306_t *display, uint8_t address, ssd1306_write_fn_t write);
void ssd1306_reset(ssd1306_t *display);
void ssd1306_present(ssd1306_t *display, const framebuffer_t *fb);

#endif
