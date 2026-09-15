#include "ringbuf.h"

void ringbuf_init(ringbuf_t *rb, uint8_t *storage, size_t capacity) {
    rb->data = storage;
    rb->capacity = capacity;
    ringbuf_reset(rb);
}

void ringbuf_reset(ringbuf_t *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->size = 0;
}

int ringbuf_push(ringbuf_t *rb, uint8_t value) {
    if (rb->size >= rb->capacity) {
        return -1;
    }

    rb->data[rb->head] = value;
    rb->head = (rb->head + 1U) % rb->capacity;
    rb->size++;
    return 0;
}

int ringbuf_pop(ringbuf_t *rb, uint8_t *value) {
    if (rb->size == 0U) {
        return -1;
    }

    *value = rb->data[rb->tail];
    rb->tail = (rb->tail + 1U) % rb->capacity;
    rb->size--;
    return 0;
}

int ringbuf_peek(const ringbuf_t *rb, uint8_t *value) {
    if (rb->size == 0U) {
        return -1;
    }

    *value = rb->data[rb->tail];
    return 0;
}

size_t ringbuf_size(const ringbuf_t *rb) {
    return rb->size;
}

size_t ringbuf_capacity(const ringbuf_t *rb) {
    return rb->capacity;
}

int ringbuf_is_empty(const ringbuf_t *rb) {
    return rb->size == 0U;
}

int ringbuf_is_full(const ringbuf_t *rb) {
    return rb->size == rb->capacity;
}
