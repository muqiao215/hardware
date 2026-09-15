#include <stdio.h>

#include "ringbuf.h"

static int expect_int(const char *label, int expected, int actual) {
    if (expected != actual) {
        fprintf(stderr, "%s: expected %d, got %d\n", label, expected, actual);
        return 0;
    }
    return 1;
}

int main(void) {
    uint8_t storage4[4];
    uint8_t storage1[1];
    ringbuf_t rb;
    ringbuf_t rb1;
    uint8_t value = 0;

    ringbuf_init(&rb, storage4, 4);
    if (!expect_int("empty", 1, ringbuf_is_empty(&rb))) {
        return 1;
    }
    if (!expect_int("capacity 4", 4, (int)ringbuf_capacity(&rb)) ||
        !expect_int("size 0", 0, (int)ringbuf_size(&rb))) {
        return 1;
    }
    if (!expect_int("pop empty", -1, ringbuf_pop(&rb, &value))) {
        return 1;
    }

    if (!expect_int("push 1", 0, ringbuf_push(&rb, 1U)) ||
        !expect_int("push 2", 0, ringbuf_push(&rb, 2U)) ||
        !expect_int("push 3", 0, ringbuf_push(&rb, 3U)) ||
        !expect_int("push 4", 0, ringbuf_push(&rb, 4U))) {
        return 1;
    }
    if (!expect_int("full", 1, ringbuf_is_full(&rb)) ||
        !expect_int("size 4", 4, (int)ringbuf_size(&rb)) ||
        !expect_int("peek first", 0, ringbuf_peek(&rb, &value)) ||
        !expect_int("peek value", 1, (int)value) ||
        !expect_int("push full", -1, ringbuf_push(&rb, 9U))) {
        return 1;
    }

    if (!expect_int("pop 1", 0, ringbuf_pop(&rb, &value)) || !expect_int("value 1", 1, (int)value) ||
        !expect_int("pop 2", 0, ringbuf_pop(&rb, &value)) || !expect_int("value 2", 2, (int)value)) {
        return 1;
    }

    if (!expect_int("push wrap 5", 0, ringbuf_push(&rb, 5U)) ||
        !expect_int("push wrap 6", 0, ringbuf_push(&rb, 6U))) {
        return 1;
    }

    if (!expect_int("pop 3", 0, ringbuf_pop(&rb, &value)) || !expect_int("value 3", 3, (int)value) ||
        !expect_int("pop 4", 0, ringbuf_pop(&rb, &value)) || !expect_int("value 4", 4, (int)value) ||
        !expect_int("pop 5", 0, ringbuf_pop(&rb, &value)) || !expect_int("value 5", 5, (int)value) ||
        !expect_int("pop 6", 0, ringbuf_pop(&rb, &value)) || !expect_int("value 6", 6, (int)value)) {
        return 1;
    }

    ringbuf_init(&rb1, storage1, 1);
    if (!expect_int("cap1 push", 0, ringbuf_push(&rb1, 7U)) ||
        !expect_int("cap1 size 1", 1, (int)ringbuf_size(&rb1)) ||
        !expect_int("cap1 full", 1, ringbuf_is_full(&rb1)) ||
        !expect_int("cap1 push full", -1, ringbuf_push(&rb1, 8U)) ||
        !expect_int("cap1 pop", 0, ringbuf_pop(&rb1, &value)) ||
        !expect_int("cap1 value", 7, (int)value) ||
        !expect_int("cap1 empty", 1, ringbuf_is_empty(&rb1))) {
        return 1;
    }

    ringbuf_reset(&rb);
    if (!expect_int("reset empty", 1, ringbuf_is_empty(&rb)) ||
        !expect_int("reset size 0", 0, (int)ringbuf_size(&rb))) {
        return 1;
    }

    if (!expect_int("interleave push 10", 0, ringbuf_push(&rb, 10U)) ||
        !expect_int("interleave pop 10", 0, ringbuf_pop(&rb, &value)) ||
        !expect_int("interleave value 10", 10, (int)value) ||
        !expect_int("interleave push 11", 0, ringbuf_push(&rb, 11U)) ||
        !expect_int("interleave push 12", 0, ringbuf_push(&rb, 12U)) ||
        !expect_int("interleave pop 11", 0, ringbuf_pop(&rb, &value)) ||
        !expect_int("interleave value 11", 11, (int)value) ||
        !expect_int("interleave push 13", 0, ringbuf_push(&rb, 13U)) ||
        !expect_int("interleave peek 12", 0, ringbuf_peek(&rb, &value)) ||
        !expect_int("interleave peek value 12", 12, (int)value) ||
        !expect_int("interleave final size", 2, (int)ringbuf_size(&rb))) {
        return 1;
    }

    printf("host ringbuf tests passed\n");
    return 0;
}
