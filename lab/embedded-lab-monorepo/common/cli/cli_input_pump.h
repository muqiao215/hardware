#ifndef CLI_INPUT_PUMP_H
#define CLI_INPUT_PUMP_H

#include <stddef.h>
#include <stdint.h>

#include "ringbuf.h"

typedef void (*cli_input_line_fn_t)(char *line, void *context);

typedef struct {
    ringbuf_t *rx_buffer;
    char *line_buffer;
    size_t line_capacity;
    size_t line_length;
    cli_input_line_fn_t line_handler;
    void *context;
} cli_input_pump_t;

void cli_input_pump_init(cli_input_pump_t *pump,
                         ringbuf_t *rx_buffer,
                         char *line_buffer,
                         size_t line_capacity,
                         cli_input_line_fn_t line_handler,
                         void *context);
int cli_input_pump_push(cli_input_pump_t *pump, char ch);
int cli_input_pump_process(cli_input_pump_t *pump);

#endif
