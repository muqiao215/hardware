/**
 * @file main.c
 * @brief Grenade Explosion Simulator - STM32F103C8T6
 *
 * Hardware:
 *   - Buzzer: PB12 (Active Low)
 *   - OLED:   PB8 (SCL), PB9 (SDA) - 4-pin I2C
 *   - LEDs:   PA0-PA7 (Active Low, running light effect)
 *
 * Effect Sequence:
 *   1. Countdown from 5 to 0 with accelerating beeps
 *   2. OLED displays countdown with dramatic text
 *   3. Final explosion: rapid LED chase + buzzer burst
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

/* ============================================================================
 * Buzzer Control (PB12, Active Low)
 * ============================================================================ */
#define BUZZER_ON()   GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define BUZZER_OFF()  GPIO_SetBits(GPIOB, GPIO_Pin_12)

void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    BUZZER_OFF();
}

void Buzzer_Beep(uint16_t ms)
{
    BUZZER_ON();
    Delay_ms(ms);
    BUZZER_OFF();
}

/* ============================================================================
 * LED Running Light (PA0-PA7, Active Low)
 * ============================================================================ */
#define LED_COUNT 8

void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = 0x00FF;  // PA0-PA7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // All LEDs off (high = off for active low)
    GPIO_Write(GPIOA, 0x00FF);
}

void LED_AllOn(void)
{
    GPIO_Write(GPIOA, 0x0000);  // Low = ON
}

void LED_AllOff(void)
{
    GPIO_Write(GPIOA, 0x00FF);  // High = OFF
}

void LED_SetPattern(uint8_t pattern)
{
    // pattern: bit=1 means LED on (active low, so invert)
    GPIO_Write(GPIOA, (uint16_t)(~pattern & 0xFF));
}

void LED_Single(uint8_t index)
{
    if (index < LED_COUNT) {
        GPIO_Write(GPIOA, ~(1 << index));
    }
}

/* ============================================================================
 * Grenade Effect Functions
 * ============================================================================ */

/**
 * @brief Countdown phase with accelerating beeps
 * @param seconds Total countdown duration
 */
void Grenade_Countdown(uint8_t seconds)
{
    // Beep intervals: start slow, get faster
    // seconds=5: 800ms, 600ms, 400ms, 200ms, 100ms intervals

    for (int8_t i = seconds; i > 0; i--) {
        // Clear and show countdown number
        OLED_Clear();

        // Line 1: Warning header
        OLED_ShowString(1, 1, "!! WARNING !!");

        // Line 2: Countdown display (large effect using multiple chars)
        OLED_ShowString(2, 1, "   T-");
        OLED_ShowNum(2, 6, i, 1);
        OLED_ShowString(2, 8, " sec");

        // Line 3: Status message
        if (i >= 4) {
            OLED_ShowString(3, 1, "  Armed...");
        } else if (i >= 2) {
            OLED_ShowString(3, 1, "  GET DOWN!");
        } else {
            OLED_ShowString(3, 1, "  DANGER!!!");
        }

        // Line 4: Progress bar
        OLED_ShowString(4, 1, "[");
        for (int j = 0; j < (seconds - i + 1) * 2; j++) {
            OLED_ShowChar(4, 2 + j, '#');
        }
        OLED_ShowString(4, 14, "]");

        // Calculate beep timing based on remaining time
        // More time left = slower beeps, less time = faster
        uint16_t beep_interval;
        uint16_t beep_duration;

        switch (i) {
            case 5:
                beep_interval = 800;
                beep_duration = 100;
                break;
            case 4:
                beep_interval = 600;
                beep_duration = 80;
                break;
            case 3:
                beep_interval = 400;
                beep_duration = 60;
                break;
            case 2:
                beep_interval = 250;
                beep_duration = 50;
                break;
            case 1:
                beep_interval = 150;
                beep_duration = 40;
                break;
            default:
                beep_interval = 100;
                beep_duration = 30;
        }

        // Multiple beeps per second, getting faster
        uint16_t elapsed = 0;
        while (elapsed < 1000) {
            Buzzer_Beep(beep_duration);
            Delay_ms(beep_interval - beep_duration);
            elapsed += beep_interval;

            // LED flash synchronized with beep
            LED_AllOn();
            Delay_ms(20);
            LED_AllOff();
        }
    }
}

/**
 * @brief Final warning - very rapid beeping
 */
void Grenade_FinalWarning(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "================");
    OLED_ShowString(2, 1, "   DETONATION   ");
    OLED_ShowString(3, 1, "   IMMINENT!!   ");
    OLED_ShowString(4, 1, "================");

    // Rapid-fire beeps
    for (int i = 0; i < 20; i++) {
        Buzzer_Beep(15);
        Delay_ms(35);

        // Rapid LED alternation
        LED_SetPattern(0x55);  // 01010101
        Delay_ms(10);
        LED_SetPattern(0xAA);  // 10101010
        Delay_ms(10);
    }
}

/**
 * @brief Explosion effect - intense LED and buzzer activity
 */
void Grenade_Explosion(void)
{
    // Screen flash effect
    OLED_Clear();
    OLED_ShowString(1, 4, "* BOOM *");
    OLED_ShowString(2, 2, "############");
    OLED_ShowString(3, 2, "############");
    OLED_ShowString(4, 4, "* BOOM *");

    // All LEDs + long buzzer burst
    LED_AllOn();
    BUZZER_ON();
    Delay_ms(500);
    BUZZER_OFF();

    // Rapid LED chase (explosion shockwave effect)
    for (int round = 0; round < 10; round++) {
        uint8_t speed = 30 - round * 2;  // Start fast, slow down
        if (speed < 10) speed = 10;

        // Outward wave
        for (int i = 0; i < LED_COUNT; i++) {
            LED_Single(i);
            Buzzer_Beep(5);
            Delay_ms(speed);
        }

        // Inward wave
        for (int i = LED_COUNT - 1; i >= 0; i--) {
            LED_Single(i);
            Buzzer_Beep(5);
            Delay_ms(speed);
        }
    }

    // Final flash burst
    for (int i = 0; i < 5; i++) {
        LED_AllOn();
        BUZZER_ON();
        Delay_ms(100);
        LED_AllOff();
        BUZZER_OFF();
        Delay_ms(100);
    }
}

/**
 * @brief Post-explosion smoke effect (fading LED pattern)
 */
void Grenade_Aftermath(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "----------------");
    OLED_ShowString(2, 3, "DETONATION");
    OLED_ShowString(3, 3, "COMPLETE");
    OLED_ShowString(4, 1, "----------------");

    // Slow random-ish LED flicker (simulating smoke/embers)
    for (int i = 0; i < 30; i++) {
        // Pseudo-random pattern using counter
        uint8_t pattern = (uint8_t)(i * 37 + 13) ^ (uint8_t)(i * 7);
        LED_SetPattern(pattern);
        Delay_ms(100);
    }

    // Fade out
    for (int i = 7; i >= 0; i--) {
        uint8_t pattern = (1 << (i + 1)) - 1;  // Decreasing number of LEDs
        LED_SetPattern(pattern);
        Delay_ms(200);
    }

    LED_AllOff();
}

/**
 * @brief Reset screen - ready for next detonation
 */
void Grenade_Reset(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "=== GRENADE ===");
    OLED_ShowString(2, 1, "Simulation v1.0");
    OLED_ShowString(3, 1, "Press RESET");
    OLED_ShowString(4, 1, "to detonate...");

    // Idle blinking
    while (1) {
        LED_Single(0);
        Delay_ms(500);
        LED_AllOff();
        Delay_ms(500);
    }
}

/* ============================================================================
 * Main Entry Point
 * ============================================================================ */
int main(void)
{
    // Initialize peripherals
    Buzzer_Init();
    LED_Init();
    OLED_Init();

    // Boot message
    OLED_Clear();
    OLED_ShowString(1, 1, "=== GRENADE ===");
    OLED_ShowString(2, 1, "Simulation v1.0");
    OLED_ShowString(3, 1, "Initializing...");

    Buzzer_Beep(100);
    Delay_ms(1000);

    // Arm the grenade
    OLED_ShowString(3, 1, ">> ARMED <<    ");
    OLED_ShowString(4, 1, "Countdown: 5s");

    Buzzer_Beep(50);
    Delay_ms(500);
    Buzzer_Beep(50);
    Delay_ms(1000);

    // Phase 1: Countdown (5 seconds, accelerating beeps)
    Grenade_Countdown(5);

    // Phase 2: Final warning (rapid beeps)
    Grenade_FinalWarning();

    // Phase 3: EXPLOSION!
    Grenade_Explosion();

    // Phase 4: Aftermath (smoke/ember effect)
    Grenade_Aftermath();

    // Phase 5: Reset state (wait for hardware reset)
    Grenade_Reset();

    // Never reached
    while (1) {
        Delay_ms(1000);
    }
}
