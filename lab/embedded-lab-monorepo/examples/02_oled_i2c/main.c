#include "delay.h"
#include "framebuffer.h"
#include "board.h"
#include "soft_i2c_bus.h"
#include "ssd1306.h"

static framebuffer_t g_framebuffer;
static ssd1306_t g_display;
static uint8_t g_oled_buffer[128 * 8];

static void draw_smiley(framebuffer_t *fb) {
    int16_t x;
    int16_t mouth_y;

    framebuffer_clear(fb);
    framebuffer_draw_circle(fb, 64, 32, 24);
    framebuffer_fill_circle(fb, 56, 25, 2);
    framebuffer_fill_circle(fb, 72, 25, 2);

    for (x = -10; x <= 10; x++) {
        mouth_y = 36 + ((100 - (x * x)) / 20);
        framebuffer_set_pixel(fb, (uint8_t)(64 + x), (uint8_t)mouth_y);
        framebuffer_set_pixel(fb, (uint8_t)(64 + x), (uint8_t)(mouth_y + 1));
    }
}

static void draw_cryface(framebuffer_t *fb, uint8_t tear_offset) {
    int16_t x;
    int16_t mouth_y;
    uint8_t tear_y = (uint8_t)(29U + (tear_offset % 8U));

    framebuffer_clear(fb);
    framebuffer_draw_circle(fb, 64, 32, 24);
    framebuffer_draw_hline(fb, 53, 59, 24);
    framebuffer_draw_hline(fb, 69, 75, 24);
    framebuffer_set_pixel(fb, 56, 25);
    framebuffer_set_pixel(fb, 72, 25);

    for (x = -10; x <= 10; x++) {
        mouth_y = 47 - ((100 - (x * x)) / 20);
        framebuffer_set_pixel(fb, (uint8_t)(64 + x), (uint8_t)mouth_y);
        framebuffer_set_pixel(fb, (uint8_t)(64 + x), (uint8_t)(mouth_y - 1));
    }

    framebuffer_set_pixel(fb, 53, tear_y);
    framebuffer_set_pixel(fb, 53, (uint8_t)(tear_y + 1U));
    framebuffer_set_pixel(fb, 54, (uint8_t)(tear_y + 1U));

    framebuffer_set_pixel(fb, 75, tear_y);
    framebuffer_set_pixel(fb, 75, (uint8_t)(tear_y + 1U));
    framebuffer_set_pixel(fb, 74, (uint8_t)(tear_y + 1U));
}

static void buzzer_init(void) {
    GPIO_InitTypeDef gpio = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

static void buzzer_on(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

static void buzzer_off(void) {
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

int main(void) {
    framebuffer_init(&g_framebuffer, 128, 64, g_oled_buffer);
    buzzer_init();
    soft_i2c_bus_init();
    delay_ms(100);
    ssd1306_init(&g_display, 0x78U, soft_i2c_bus_write);
    ssd1306_reset(&g_display);
    framebuffer_clear(&g_framebuffer);
    ssd1306_present(&g_display, &g_framebuffer);

    while (1) {
        draw_smiley(&g_framebuffer);
        ssd1306_present(&g_display, &g_framebuffer);
        buzzer_on();
        delay_ms(900);
        buzzer_off();

        draw_cryface(&g_framebuffer, 0);
        ssd1306_present(&g_display, &g_framebuffer);
        buzzer_on();
        delay_ms(500);
        buzzer_off();

        draw_cryface(&g_framebuffer, 4);
        ssd1306_present(&g_display, &g_framebuffer);
        buzzer_on();
        delay_ms(500);
        buzzer_off();
    }
}
