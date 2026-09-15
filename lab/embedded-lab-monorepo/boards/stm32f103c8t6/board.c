#include "board.h"

void board_led_init(void) {
    GPIO_InitTypeDef gpio = {0};

    RCC_APB2PeriphClockCmd(BOARD_LED_CLOCK, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = BOARD_LED_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BOARD_LED_GPIO, &gpio);
    board_led_off();
}

void board_led_on(void) {
    GPIO_ResetBits(BOARD_LED_GPIO, BOARD_LED_PIN);
}

void board_led_off(void) {
    GPIO_SetBits(BOARD_LED_GPIO, BOARD_LED_PIN);
}

void board_led_toggle(void) {
    if (GPIO_ReadOutputDataBit(BOARD_LED_GPIO, BOARD_LED_PIN) == Bit_SET) {
        board_led_on();
    } else {
        board_led_off();
    }
}

void board_button_input_init(void) {
    GPIO_InitTypeDef gpio = {0};

    RCC_APB2PeriphClockCmd(BOARD_KEY_CLOCK | RCC_APB2Periph_AFIO, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Pin = BOARD_KEY_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BOARD_KEY_GPIO, &gpio);
}
