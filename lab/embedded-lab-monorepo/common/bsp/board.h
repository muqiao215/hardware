#ifndef BOARD_H
#define BOARD_H

#include "stm32f10x.h"

#define BOARD_LED_GPIO GPIOC
#define BOARD_LED_PIN GPIO_Pin_13
#define BOARD_LED_CLOCK RCC_APB2Periph_GPIOC

#define BOARD_KEY_GPIO GPIOA
#define BOARD_KEY_PIN GPIO_Pin_0
#define BOARD_KEY_CLOCK RCC_APB2Periph_GPIOA
#define BOARD_KEY_EXTI_PORTSOURCE GPIO_PortSourceGPIOA
#define BOARD_KEY_EXTI_PINSOURCE GPIO_PinSource0
#define BOARD_KEY_EXTI_LINE EXTI_Line0
#define BOARD_KEY_IRQn EXTI0_IRQn

#define BOARD_USART_GPIO GPIOA
#define BOARD_USART_TX_PIN GPIO_Pin_9
#define BOARD_USART_RX_PIN GPIO_Pin_10

#define BOARD_SOFT_I2C_GPIO GPIOB
#define BOARD_SOFT_I2C_CLOCK RCC_APB2Periph_GPIOB
#define BOARD_SOFT_I2C_SCL_PIN GPIO_Pin_8
#define BOARD_SOFT_I2C_SDA_PIN GPIO_Pin_9

#define BOARD_I2C_GPIO GPIOB
#define BOARD_I2C_CLOCK RCC_APB2Periph_GPIOB
#define BOARD_I2C_SCL_PIN GPIO_Pin_6
#define BOARD_I2C_SDA_PIN GPIO_Pin_7

#define BOARD_SPI_GPIO GPIOA
#define BOARD_SPI_CLOCK RCC_APB2Periph_GPIOA
#define BOARD_SPI_SCK_PIN GPIO_Pin_5
#define BOARD_SPI_MISO_PIN GPIO_Pin_6
#define BOARD_SPI_MOSI_PIN GPIO_Pin_7
#define BOARD_SPI_NSS_PIN GPIO_Pin_4

void board_led_init(void);
void board_led_on(void);
void board_led_off(void);
void board_led_toggle(void);
void board_button_input_init(void);

#endif
