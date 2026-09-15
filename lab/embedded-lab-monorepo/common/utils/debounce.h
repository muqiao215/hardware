#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <stdint.h>

typedef struct {
    uint8_t stable_state;
    uint8_t candidate_state;
    uint8_t count;
    uint8_t threshold;
    uint8_t pressed_event;
    uint8_t released_event;
} debounce_t;

void debounce_init(debounce_t *db, uint8_t initial_state, uint8_t threshold);
void debounce_reset(debounce_t *db, uint8_t initial_state);
void debounce_update(debounce_t *db, uint8_t sample_state);
uint8_t debounce_is_pressed(const debounce_t *db);
uint8_t debounce_is_released(const debounce_t *db);
uint8_t debounce_pressed_event(debounce_t *db);
uint8_t debounce_released_event(debounce_t *db);

#endif
