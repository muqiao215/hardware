#include <stdio.h>

#include "framebuffer.h"

static int expect_bit(const framebuffer_t *fb, unsigned int x, unsigned int y) {
    unsigned int index = x + (y / 8U) * fb->width;
    unsigned int mask = 1U << (y % 8U);

    return (fb->data[index] & mask) != 0U;
}

int main(void) {
    uint8_t buffer[128] = {0};
    framebuffer_t fb;

    framebuffer_init(&fb, 16, 64, buffer);
    framebuffer_clear(&fb);

    framebuffer_set_pixel(&fb, 1, 2);
    if (!expect_bit(&fb, 1, 2)) {
        fprintf(stderr, "single pixel draw failed\n");
        return 1;
    }

    framebuffer_draw_hline(&fb, 2, 5, 7);
    if (!expect_bit(&fb, 2, 7) || !expect_bit(&fb, 5, 7)) {
        fprintf(stderr, "horizontal line draw failed\n");
        return 1;
    }

    framebuffer_draw_circle(&fb, 8, 16, 3);
    if (!expect_bit(&fb, 11, 16) || !expect_bit(&fb, 5, 16) ||
        !expect_bit(&fb, 8, 19) || !expect_bit(&fb, 8, 13)) {
        fprintf(stderr, "circle draw failed\n");
        return 1;
    }

    framebuffer_fill_circle(&fb, 8, 32, 2);
    if (!expect_bit(&fb, 8, 32) || !expect_bit(&fb, 7, 32) || !expect_bit(&fb, 8, 31)) {
        fprintf(stderr, "filled circle draw failed\n");
        return 1;
    }

    printf("host framebuffer tests passed\n");
    return 0;
}
