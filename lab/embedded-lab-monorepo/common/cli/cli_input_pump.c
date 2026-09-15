#include "cli_input_pump.h"

void cli_input_pump_init(cli_input_pump_t *pump,
                         ringbuf_t *rx_buffer,
                         char *line_buffer,
                         size_t line_capacity,
                         cli_input_line_fn_t line_handler,
                         void *context) {
    pump->rx_buffer = rx_buffer;
    pump->line_buffer = line_buffer;
    pump->line_capacity = line_capacity;
    pump->line_length = 0;
    pump->line_handler = line_handler;
    pump->context = context;
}

int cli_input_pump_push(cli_input_pump_t *pump, char ch) {
    return ringbuf_push(pump->rx_buffer, (uint8_t)ch);
}

int cli_input_pump_process(cli_input_pump_t *pump) {
    uint8_t value = 0;

    while (ringbuf_pop(pump->rx_buffer, &value) == 0) {
        if (value == '\r' || value == '\n') {
            pump->line_buffer[pump->line_length] = '\0';
            pump->line_length = 0;
            pump->line_handler(pump->line_buffer, pump->context);
            return 1;
        }

        if (pump->line_length + 1U >= pump->line_capacity) {
            pump->line_length = 0;
            return -1;
        }

        pump->line_buffer[pump->line_length++] = (char)value;
    }

    return 0;
}
