#include "framebuffer.h"

void framebuffer_init(framebuffer_t *fb, uint16_t width, uint16_t height, uint8_t *data) {
    fb->width = width;
    fb->height = height;
    fb->data = data;
}

void framebuffer_clear(framebuffer_t *fb) {
    uint16_t count = (uint16_t)(fb->width * (fb->height / 8U));
    uint16_t index;

    for (index = 0; index < count; index++) {
        fb->data[index] = 0;
    }
}

void framebuffer_set_pixel(framebuffer_t *fb, uint8_t x, uint8_t y) {
    uint16_t index;

    if (x >= fb->width || y >= fb->height) {
        return;
    }

    index = x + (uint16_t)(y / 8U) * fb->width;
    fb->data[index] |= (uint8_t)(1U << (y % 8U));
}

void framebuffer_draw_hline(framebuffer_t *fb, uint8_t x0, uint8_t x1, uint8_t y) {
    uint8_t x;

    if (x0 > x1) {
        uint8_t tmp = x0;
        x0 = x1;
        x1 = tmp;
    }

    for (x = x0; x <= x1; x++) {
        framebuffer_set_pixel(fb, x, y);
    }
}

void framebuffer_draw_circle(framebuffer_t *fb, uint8_t cx, uint8_t cy, uint8_t radius) {
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 1 - x;

    while (x >= y) {
        framebuffer_set_pixel(fb, (uint8_t)(cx + x), (uint8_t)(cy + y));
        framebuffer_set_pixel(fb, (uint8_t)(cx + y), (uint8_t)(cy + x));
        framebuffer_set_pixel(fb, (uint8_t)(cx - y), (uint8_t)(cy + x));
        framebuffer_set_pixel(fb, (uint8_t)(cx - x), (uint8_t)(cy + y));
        framebuffer_set_pixel(fb, (uint8_t)(cx - x), (uint8_t)(cy - y));
        framebuffer_set_pixel(fb, (uint8_t)(cx - y), (uint8_t)(cy - x));
        framebuffer_set_pixel(fb, (uint8_t)(cx + y), (uint8_t)(cy - x));
        framebuffer_set_pixel(fb, (uint8_t)(cx + x), (uint8_t)(cy - y));

        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void framebuffer_fill_circle(framebuffer_t *fb, uint8_t cx, uint8_t cy, uint8_t radius) {
    int16_t x;
    int16_t y;
    int16_t rr = (int16_t)radius * (int16_t)radius;

    for (y = -(int16_t)radius; y <= (int16_t)radius; y++) {
        for (x = -(int16_t)radius; x <= (int16_t)radius; x++) {
            if ((x * x + y * y) <= rr) {
                framebuffer_set_pixel(fb, (uint8_t)(cx + x), (uint8_t)(cy + y));
            }
        }
    }
}
