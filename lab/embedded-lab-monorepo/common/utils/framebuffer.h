#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t *data;
} framebuffer_t;

void framebuffer_init(framebuffer_t *fb, uint16_t width, uint16_t height, uint8_t *data);
void framebuffer_clear(framebuffer_t *fb);
void framebuffer_set_pixel(framebuffer_t *fb, uint8_t x, uint8_t y);
void framebuffer_draw_hline(framebuffer_t *fb, uint8_t x0, uint8_t x1, uint8_t y);
void framebuffer_draw_circle(framebuffer_t *fb, uint8_t cx, uint8_t cy, uint8_t radius);
void framebuffer_fill_circle(framebuffer_t *fb, uint8_t cx, uint8_t cy, uint8_t radius);

#endif
