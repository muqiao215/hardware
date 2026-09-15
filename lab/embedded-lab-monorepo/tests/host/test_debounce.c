#include <stdio.h>

#include "debounce.h"

static int expect_int(const char *label, int expected, int actual) {
    if (expected != actual) {
        fprintf(stderr, "%s: expected %d, got %d\n", label, expected, actual);
        return 0;
    }
    return 1;
}

int main(void) {
    debounce_t db;
    debounce_t db_fast;

    debounce_init(&db, 0U, 3U);
    debounce_update(&db, 1U);
    debounce_update(&db, 0U);
    debounce_update(&db, 1U);
    if (!expect_int("no early press", 0, debounce_pressed_event(&db))) {
        return 1;
    }

    debounce_update(&db, 1U);
    debounce_update(&db, 1U);
    if (!expect_int("press event", 1, debounce_pressed_event(&db)) ||
        !expect_int("stable pressed", 1, debounce_is_pressed(&db)) ||
        !expect_int("press event clear", 0, debounce_pressed_event(&db))) {
        return 1;
    }

    debounce_update(&db, 0U);
    debounce_update(&db, 1U);
    debounce_update(&db, 0U);
    if (!expect_int("no early release", 0, debounce_released_event(&db))) {
        return 1;
    }

    debounce_update(&db, 0U);
    debounce_update(&db, 0U);
    if (!expect_int("release event", 1, debounce_released_event(&db)) ||
        !expect_int("stable released", 1, debounce_is_released(&db)) ||
        !expect_int("release event clear", 0, debounce_released_event(&db))) {
        return 1;
    }

    debounce_init(&db_fast, 0U, 2U);
    debounce_update(&db_fast, 1U);
    debounce_update(&db_fast, 0U);
    debounce_update(&db_fast, 1U);
    debounce_update(&db_fast, 0U);
    if (!expect_int("bounce stays released", 1, debounce_is_released(&db_fast)) ||
        !expect_int("bounce no press event", 0, debounce_pressed_event(&db_fast))) {
        return 1;
    }

    debounce_update(&db_fast, 1U);
    debounce_update(&db_fast, 1U);
    if (!expect_int("fast press event", 1, debounce_pressed_event(&db_fast)) ||
        !expect_int("fast pressed", 1, debounce_is_pressed(&db_fast))) {
        return 1;
    }

    debounce_reset(&db_fast, 1U);
    if (!expect_int("reset pressed", 1, debounce_is_pressed(&db_fast)) ||
        !expect_int("reset no stale release", 0, debounce_released_event(&db_fast))) {
        return 1;
    }

    printf("host debounce tests passed\n");
    return 0;
}
