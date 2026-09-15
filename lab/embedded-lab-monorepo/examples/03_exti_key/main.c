#include "board.h"
#include "debounce.h"
#include "delay.h"

volatile uint32_t g_exti_count = 0;
static debounce_t g_key_debounce;

void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(BOARD_KEY_EXTI_LINE) != RESET) {
        g_exti_count++;
        EXTI_ClearITPendingBit(BOARD_KEY_EXTI_LINE);
    }
}

static void exti_key_init(void) {
    EXTI_InitTypeDef exti = {0};
    NVIC_InitTypeDef nvic = {0};

    board_button_input_init();

    GPIO_EXTILineConfig(BOARD_KEY_EXTI_PORTSOURCE, BOARD_KEY_EXTI_PINSOURCE);
    exti.EXTI_Line = BOARD_KEY_EXTI_LINE;
    exti.EXTI_LineCmd = ENABLE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&exti);

    nvic.NVIC_IRQChannel = BOARD_KEY_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&nvic);
}

int main(void) {
    board_led_init();
    exti_key_init();
    debounce_init(&g_key_debounce, 0U, 20U);

    while (1) {
        uint8_t raw_pressed = (uint8_t)(GPIO_ReadInputDataBit(BOARD_KEY_GPIO, BOARD_KEY_PIN) == Bit_RESET);

        debounce_update(&g_key_debounce, raw_pressed);
        if (debounce_pressed_event(&g_key_debounce) != 0U) {
            board_led_toggle();
        }
        delay_ms(1);
    }
}
