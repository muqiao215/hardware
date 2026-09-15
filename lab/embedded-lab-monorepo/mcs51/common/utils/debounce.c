#include "debounce.h"

void debounce_init(debounce_t *debounce, uint8_t initial_level, uint8_t threshold) {
    debounce->stable_level = initial_level ? 1U : 0U;
    debounce->candidate_level = debounce->stable_level;
    debounce->threshold = threshold == 0U ? 1U : threshold;
    debounce->count = 0U;
    debounce->pressed_event = 0U;
    debounce->released_event = 0U;
}

void debounce_update(debounce_t *debounce, uint8_t raw_level) {
    uint8_t level = raw_level ? 1U : 0U;

    debounce->pressed_event = 0U;
    debounce->released_event = 0U;

    if (level == debounce->stable_level) {
        debounce->candidate_level = level;
        debounce->count = 0U;
        return;
    }

    if (level != debounce->candidate_level) {
        debounce->candidate_level = level;
        debounce->count = 1U;
        return;
    }

    if (debounce->count < debounce->threshold) {
        debounce->count++;
    }

    if (debounce->count >= debounce->threshold) {
        debounce->stable_level = level;
        debounce->count = 0U;

        if (level != 0U) {
            debounce->pressed_event = 1U;
        } else {
            debounce->released_event = 1U;
        }
    }
}

uint8_t debounce_is_pressed(const debounce_t *debounce) {
    return debounce->stable_level;
}

uint8_t debounce_pressed_event(const debounce_t *debounce) {
    return debounce->pressed_event;
}

uint8_t debounce_released_event(const debounce_t *debounce) {
    return debounce->released_event;
}
