#ifndef RINGBUF_H
#define RINGBUF_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
} ringbuf_t;

void ringbuf_init(ringbuf_t *rb, uint8_t *storage, size_t capacity);
void ringbuf_reset(ringbuf_t *rb);
int ringbuf_push(ringbuf_t *rb, uint8_t value);
int ringbuf_pop(ringbuf_t *rb, uint8_t *value);
int ringbuf_peek(const ringbuf_t *rb, uint8_t *value);
size_t ringbuf_size(const ringbuf_t *rb);
size_t ringbuf_capacity(const ringbuf_t *rb);
int ringbuf_is_empty(const ringbuf_t *rb);
int ringbuf_is_full(const ringbuf_t *rb);

#endif
