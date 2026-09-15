#include "debounce.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "assert failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); \
        exit(1); \
    } \
} while (0)

static uint8_t run_sequence(const uint8_t *samples, uint8_t count, uint8_t *presses, uint8_t *releases) {
    debounce_t debounce;
    uint8_t i;

    debounce_init(&debounce, 0U, 3U);
    *presses = 0U;
    *releases = 0U;

    for (i = 0U; i < count; i++) {
        debounce_update(&debounce, samples[i]);
        if (debounce_pressed_event(&debounce)) {
            (*presses)++;
        }
        if (debounce_released_event(&debounce)) {
            (*releases)++;
        }
    }

    return debounce_is_pressed(&debounce);
}

static void test_clean_press_and_release(void) {
    const uint8_t samples[] = {0U, 1U, 1U, 1U, 0U, 0U, 0U};
    uint8_t presses;
    uint8_t releases;
    uint8_t pressed = run_sequence(samples, sizeof(samples), &presses, &releases);

    ASSERT_TRUE(pressed == 0U);
    ASSERT_TRUE(presses == 1U);
    ASSERT_TRUE(releases == 1U);
}

static void test_press_bounce_triggers_once(void) {
    const uint8_t samples[] = {0U, 1U, 0U, 1U, 1U, 1U, 1U, 1U};
    uint8_t presses;
    uint8_t releases;
    uint8_t pressed = run_sequence(samples, sizeof(samples), &presses, &releases);

    ASSERT_TRUE(pressed == 1U);
    ASSERT_TRUE(presses == 1U);
    ASSERT_TRUE(releases == 0U);
}

static void test_release_bounce_triggers_once(void) {
    const uint8_t samples[] = {1U, 1U, 1U, 0U, 1U, 0U, 0U, 0U, 0U};
    uint8_t presses;
    uint8_t releases;
    uint8_t pressed = run_sequence(samples, sizeof(samples), &presses, &releases);

    ASSERT_TRUE(pressed == 0U);
    ASSERT_TRUE(presses == 1U);
    ASSERT_TRUE(releases == 1U);
}

static void test_held_press_does_not_repeat(void) {
    const uint8_t samples[] = {1U, 1U, 1U, 1U, 1U, 1U, 1U};
    uint8_t presses;
    uint8_t releases;
    uint8_t pressed = run_sequence(samples, sizeof(samples), &presses, &releases);

    ASSERT_TRUE(pressed == 1U);
    ASSERT_TRUE(presses == 1U);
    ASSERT_TRUE(releases == 0U);
}

static void test_idle_does_not_trigger(void) {
    const uint8_t samples[] = {0U, 0U, 0U, 0U, 0U, 0U};
    uint8_t presses;
    uint8_t releases;
    uint8_t pressed = run_sequence(samples, sizeof(samples), &presses, &releases);

    ASSERT_TRUE(pressed == 0U);
    ASSERT_TRUE(presses == 0U);
    ASSERT_TRUE(releases == 0U);
}

static void test_noise_does_not_trigger(void) {
    const uint8_t samples[] = {0U, 1U, 0U, 1U, 0U, 1U, 0U, 0U};
    uint8_t presses;
    uint8_t releases;
    uint8_t pressed = run_sequence(samples, sizeof(samples), &presses, &releases);

    ASSERT_TRUE(pressed == 0U);
    ASSERT_TRUE(presses == 0U);
    ASSERT_TRUE(releases == 0U);
}

int main(void) {
    test_clean_press_and_release();
    test_press_bounce_triggers_once();
    test_release_bounce_triggers_once();
    test_held_press_does_not_repeat();
    test_idle_does_not_trigger();
    test_noise_does_not_trigger();

    puts("mcs51 host debounce tests passed");
    return 0;
}
