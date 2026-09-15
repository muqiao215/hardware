#ifndef MCS51_DEBOUNCE_H
#define MCS51_DEBOUNCE_H

#include <stdint.h>

typedef struct {
    uint8_t stable_level;
    uint8_t candidate_level;
    uint8_t threshold;
    uint8_t count;
    uint8_t pressed_event;
    uint8_t released_event;
} debounce_t;

void debounce_init(debounce_t *debounce, uint8_t initial_level, uint8_t threshold);
void debounce_update(debounce_t *debounce, uint8_t raw_level);
uint8_t debounce_is_pressed(const debounce_t *debounce);
uint8_t debounce_pressed_event(const debounce_t *debounce);
uint8_t debounce_released_event(const debounce_t *debounce);

#endif
