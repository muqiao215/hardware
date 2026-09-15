#include "ssd1306.h"

static void ssd1306_write_command(ssd1306_t *display, uint8_t command) {
    uint8_t packet[2];

    packet[0] = 0x00U;
    packet[1] = command;
    display->write(display->address, packet, 2);
}

static void ssd1306_set_cursor(ssd1306_t *display, uint8_t page, uint8_t column) {
    ssd1306_write_command(display, (uint8_t)(0xB0U | page));
    ssd1306_write_command(display, (uint8_t)(0x10U | ((column & 0xF0U) >> 4)));
    ssd1306_write_command(display, (uint8_t)(0x00U | (column & 0x0FU)));
}

void ssd1306_init(ssd1306_t *display, uint8_t address, ssd1306_write_fn_t write) {
    display->address = address;
    display->write = write;
}

void ssd1306_reset(ssd1306_t *display) {
    static const uint8_t init_sequence[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0xA1, 0xC8, 0xDA, 0x12, 0x81, 0xCF, 0xD9, 0xF1,
        0xDB, 0x30, 0xA4, 0xA6, 0x8D, 0x14, 0x20, 0x10,
        0x21, 0x00, 0x7F, 0x22, 0x00, 0x07, 0xAF
    };
    uint16_t index;

    for (index = 0; index < sizeof(init_sequence); index++) {
        ssd1306_write_command(display, init_sequence[index]);
    }
}

void ssd1306_present(ssd1306_t *display, const framebuffer_t *fb) {
    uint8_t page;
    uint8_t packet[129];
    uint16_t page_offset = 0;
    uint8_t column;

    packet[0] = 0x40U;

    for (page = 0; page < (fb->height / 8U); page++) {
        ssd1306_set_cursor(display, page, 0);
        for (column = 0; column < fb->width; column++) {
            packet[column + 1U] = fb->data[page_offset + column];
        }
        display->write(display->address, packet, (uint16_t)(fb->width + 1U));
        page_offset += fb->width;
    }
}
