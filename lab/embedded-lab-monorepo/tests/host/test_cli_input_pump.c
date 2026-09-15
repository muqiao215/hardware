#include <stdio.h>

#include "cli_input_pump.h"
#include "ringbuf.h"

static char g_last_line[64];
static int g_handler_calls = 0;

static int expect_int(const char *label, int expected, int actual) {
    if (expected != actual) {
        fprintf(stderr, "%s: expected %d, got %d\n", label, expected, actual);
        return 0;
    }
    return 1;
}

static int strings_equal(const char *lhs, const char *rhs) {
    while (*lhs != '\0' || *rhs != '\0') {
        if (*lhs != *rhs) {
            return 0;
        }
        lhs++;
        rhs++;
    }
    return 1;
}

static void reset_capture(void) {
    int index;

    g_handler_calls = 0;
    for (index = 0; index < (int)sizeof(g_last_line); index++) {
        g_last_line[index] = '\0';
    }
}

static void capture_line(char *line, void *context) {
    int index = 0;
    char *destination = (char *)context;

    g_handler_calls++;
    while (line[index] != '\0' && index + 1 < (int)sizeof(g_last_line)) {
        destination[index] = line[index];
        index++;
    }
    destination[index] = '\0';
}

static int push_text(cli_input_pump_t *pump, const char *text) {
    while (*text != '\0') {
        if (cli_input_pump_push(pump, *text) != 0) {
            return -1;
        }
        text++;
    }
    return 0;
}

int main(void) {
    uint8_t rx_storage[32];
    char line_buffer[16];
    ringbuf_t rb;
    cli_input_pump_t pump;

    ringbuf_init(&rb, rx_storage, sizeof(rx_storage));
    cli_input_pump_init(&pump, &rb, line_buffer, sizeof(line_buffer), capture_line, g_last_line);

    reset_capture();
    if (!expect_int("push hello", 0, push_text(&pump, "help")) ||
        !expect_int("process partial", 0, cli_input_pump_process(&pump)) ||
        !expect_int("no handler yet", 0, g_handler_calls) ||
        !expect_int("push cr", 0, cli_input_pump_push(&pump, '\r')) ||
        !expect_int("process complete", 1, cli_input_pump_process(&pump)) ||
        !expect_int("handler once", 1, g_handler_calls) ||
        !expect_int("line matches", 1, strings_equal(g_last_line, "help"))) {
        return 1;
    }

    reset_capture();
    if (!expect_int("empty line push lf", 0, cli_input_pump_push(&pump, '\n')) ||
        !expect_int("empty line process", 1, cli_input_pump_process(&pump)) ||
        !expect_int("empty handler once", 1, g_handler_calls) ||
        !expect_int("empty line text", 1, strings_equal(g_last_line, ""))) {
        return 1;
    }

    reset_capture();
    if (!expect_int("push spaced text", 0, push_text(&pump, "  echo   hi  ")) ||
        !expect_int("push lf", 0, cli_input_pump_push(&pump, '\n')) ||
        !expect_int("process spaced text", 1, cli_input_pump_process(&pump)) ||
        !expect_int("spaced handler once", 1, g_handler_calls) ||
        !expect_int("spaced line kept", 1, strings_equal(g_last_line, "  echo   hi  "))) {
        return 1;
    }

    reset_capture();
    if (!expect_int("push too long", 0, push_text(&pump, "0123456789abcdef")) ||
        !expect_int("overflow detected", -1, cli_input_pump_process(&pump))) {
        return 1;
    }

    ringbuf_reset(&rb);
    reset_capture();
    if (!expect_int("push second line", 0, push_text(&pump, "echo ok\r")) ||
        !expect_int("process second line", 1, cli_input_pump_process(&pump)) ||
        !expect_int("handler second line", 1, g_handler_calls) ||
        !expect_int("second line text", 1, strings_equal(g_last_line, "echo ok"))) {
        return 1;
    }

    printf("host cli input pump tests passed\n");
    return 0;
}
