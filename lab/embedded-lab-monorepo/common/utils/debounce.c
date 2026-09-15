#include "debounce.h"

void debounce_init(debounce_t *db, uint8_t initial_state, uint8_t threshold) {
    db->threshold = threshold;
    debounce_reset(db, initial_state);
}

void debounce_reset(debounce_t *db, uint8_t initial_state) {
    db->stable_state = initial_state;
    db->candidate_state = initial_state;
    db->count = 0;
    db->pressed_event = 0;
    db->released_event = 0;
}

void debounce_update(debounce_t *db, uint8_t sample_state) {
    db->pressed_event = 0;
    db->released_event = 0;

    if (sample_state == db->stable_state) {
        db->candidate_state = sample_state;
        db->count = 0;
        return;
    }

    if (sample_state != db->candidate_state) {
        db->candidate_state = sample_state;
        db->count = 1;
        return;
    }

    if (db->count < db->threshold) {
        db->count++;
    }

    if (db->count >= db->threshold) {
        db->stable_state = db->candidate_state;
        db->count = 0;
        if (db->stable_state != 0U) {
            db->pressed_event = 1;
        } else {
            db->released_event = 1;
        }
    }
}

uint8_t debounce_is_pressed(const debounce_t *db) {
    return db->stable_state != 0U;
}

uint8_t debounce_is_released(const debounce_t *db) {
    return db->stable_state == 0U;
}

uint8_t debounce_pressed_event(debounce_t *db) {
    uint8_t value = db->pressed_event;
    db->pressed_event = 0;
    return value;
}

uint8_t debounce_released_event(debounce_t *db) {
    uint8_t value = db->released_event;
    db->released_event = 0;
    return value;
}
