#include "bsp_delay.h"
#include "drv_ssd1306.h"
#include "drv_soft_i2c.h"
#include "oled.h"

/* Transitional include keeps the narrow split buildable before CMake is widened. */
#include "bsp_delay.c"
/* Transitional include keeps the narrow split buildable before CMake is widened. */
#include "drv_soft_i2c.c"
/* Transitional include keeps the narrow split buildable before CMake is widened. */
#include "drv_ssd1306.c"

static uint8_t oled_buffer[128 * 8];

static void oled_draw_pixel(uint8_t x, uint8_t y) {
    uint16_t index;

    if (x >= 128 || y >= 64) {
        return;
    }

    index = x + (uint16_t)(y / 8U) * 128U;
    oled_buffer[index] |= (uint8_t)(1U << (y % 8U));
}

static void oled_fill_circle(uint8_t cx, uint8_t cy, uint8_t radius) {
    int16_t x;
    int16_t y;
    int16_t rr = (int16_t)radius * (int16_t)radius;

    for (y = -(int16_t)radius; y <= (int16_t)radius; y++) {
        for (x = -(int16_t)radius; x <= (int16_t)radius; x++) {
            if ((x * x + y * y) <= rr) {
                oled_draw_pixel((uint8_t)(cx + x), (uint8_t)(cy + y));
            }
        }
    }
}

static void oled_draw_circle(uint8_t cx, uint8_t cy, uint8_t radius) {
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 1 - x;

    while (x >= y) {
        oled_draw_pixel((uint8_t)(cx + x), (uint8_t)(cy + y));
        oled_draw_pixel((uint8_t)(cx + y), (uint8_t)(cy + x));
        oled_draw_pixel((uint8_t)(cx - y), (uint8_t)(cy + x));
        oled_draw_pixel((uint8_t)(cx - x), (uint8_t)(cy + y));
        oled_draw_pixel((uint8_t)(cx - x), (uint8_t)(cy - y));
        oled_draw_pixel((uint8_t)(cx - y), (uint8_t)(cy - x));
        oled_draw_pixel((uint8_t)(cx + y), (uint8_t)(cy - x));
        oled_draw_pixel((uint8_t)(cx + x), (uint8_t)(cy - y));

        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void oled_clear(void) {
    uint16_t index;

    for (index = 0; index < sizeof(oled_buffer); index++) {
        oled_buffer[index] = 0;
    }
}

static void oled_draw_hline(uint8_t x0, uint8_t x1, uint8_t y) {
    uint8_t x;

    if (x0 > x1) {
        uint8_t tmp = x0;
        x0 = x1;
        x1 = tmp;
    }

    for (x = x0; x <= x1; x++) {
        oled_draw_pixel(x, y);
    }
}

void oled_draw_smiley(void) {
    int16_t x;
    int16_t mouth_y;

    oled_clear();
    oled_draw_circle(64, 32, 24);
    oled_fill_circle(56, 25, 2);
    oled_fill_circle(72, 25, 2);

    for (x = -10; x <= 10; x++) {
        mouth_y = 36 + ((100 - (x * x)) / 20);
        oled_draw_pixel((uint8_t)(64 + x), (uint8_t)mouth_y);
        oled_draw_pixel((uint8_t)(64 + x), (uint8_t)(mouth_y + 1));
    }
}

void oled_draw_cryface(uint8_t tear_offset) {
    int16_t x;
    int16_t mouth_y;
    uint8_t tear_y = (uint8_t)(29U + (tear_offset % 8U));

    oled_clear();
    oled_draw_circle(64, 32, 24);

    oled_draw_hline(53, 59, 24);
    oled_draw_hline(69, 75, 24);
    oled_draw_pixel(56, 25);
    oled_draw_pixel(72, 25);

    for (x = -10; x <= 10; x++) {
        mouth_y = 47 - ((100 - (x * x)) / 20);
        oled_draw_pixel((uint8_t)(64 + x), (uint8_t)mouth_y);
        oled_draw_pixel((uint8_t)(64 + x), (uint8_t)(mouth_y - 1));
    }

    oled_draw_pixel(53, tear_y);
    oled_draw_pixel(53, (uint8_t)(tear_y + 1U));
    oled_draw_pixel(54, (uint8_t)(tear_y + 1U));

    oled_draw_pixel(75, tear_y);
    oled_draw_pixel(75, (uint8_t)(tear_y + 1U));
    oled_draw_pixel(74, (uint8_t)(tear_y + 1U));
}

void oled_update(void) {
    uint8_t page;
    uint8_t column;

    for (page = 0; page < 8; page++) {
        drv_ssd1306_set_cursor(page, 0);
        for (column = 0; column < 128; column++) {
            drv_ssd1306_write_data(oled_buffer[page * 128U + column]);
        }
    }
}

void oled_init(void) {
    drv_soft_i2c_init();
    bsp_delay_ms(100);

    drv_ssd1306_write_command(0xAE);
    drv_ssd1306_write_command(0xD5);
    drv_ssd1306_write_command(0x80);
    drv_ssd1306_write_command(0xA8);
    drv_ssd1306_write_command(0x3F);
    drv_ssd1306_write_command(0xD3);
    drv_ssd1306_write_command(0x00);
    drv_ssd1306_write_command(0x40);
    drv_ssd1306_write_command(0xA1);
    drv_ssd1306_write_command(0xC8);
    drv_ssd1306_write_command(0xDA);
    drv_ssd1306_write_command(0x12);
    drv_ssd1306_write_command(0x81);
    drv_ssd1306_write_command(0xCF);
    drv_ssd1306_write_command(0xD9);
    drv_ssd1306_write_command(0xF1);
    drv_ssd1306_write_command(0xDB);
    drv_ssd1306_write_command(0x30);
    drv_ssd1306_write_command(0xA4);
    drv_ssd1306_write_command(0xA6);
    drv_ssd1306_write_command(0x8D);
    drv_ssd1306_write_command(0x14);
    drv_ssd1306_write_command(0x20);
    drv_ssd1306_write_command(0x10);
    drv_ssd1306_write_command(0x21);
    drv_ssd1306_write_command(0x00);
    drv_ssd1306_write_command(0x7F);
    drv_ssd1306_write_command(0x22);
    drv_ssd1306_write_command(0x00);
    drv_ssd1306_write_command(0x07);
    drv_ssd1306_write_command(0xAF);

    oled_clear();
    oled_update();
}
