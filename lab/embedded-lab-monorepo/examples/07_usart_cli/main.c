#include "board.h"
#include "cli_dispatch.h"
#include "cli_input_pump.h"
#include "cli_parser.h"
#include "delay.h"
#include "usart_cli_port.h"
#include <stddef.h>

#define CLI_MAX_ARGS 4
#define CLI_LINE_BUFFER_SIZE 64

static uint8_t g_cli_rx_storage[CLI_LINE_BUFFER_SIZE];
static char g_cli_line[CLI_LINE_BUFFER_SIZE];
static ringbuf_t g_cli_rx_buffer;
static cli_input_pump_t g_cli_pump;

static void cli_write(const char *text, void *context) {
    (void)context;
    usart_cli_port_write_string(text);
}

static int cli_help_handler(int argc, char **argv, cli_write_fn_t write, void *context);

static int cli_led_handler(int argc, char **argv, cli_write_fn_t write, void *context) {
    (void)context;

    if (argc < 2) {
        write("usage: led on|off|toggle\r\n", context);
        return -1;
    }

    if (argv[1][0] == 'o' && argv[1][1] == 'n' && argv[1][2] == '\0') {
        board_led_on();
    } else if (argv[1][0] == 'o' && argv[1][1] == 'f' && argv[1][2] == 'f' && argv[1][3] == '\0') {
        board_led_off();
    } else if (argv[1][0] == 't' && argv[1][1] == 'o' && argv[1][2] == 'g' &&
               argv[1][3] == 'g' && argv[1][4] == 'l' && argv[1][5] == 'e' && argv[1][6] == '\0') {
        board_led_toggle();
    } else {
        write("error: invalid led action\r\n", context);
        return -1;
    }

    write("ok\r\n", context);
    return 0;
}

static int cli_echo_handler(int argc, char **argv, cli_write_fn_t write, void *context) {
    int index;

    for (index = 1; index < argc; index++) {
        write(argv[index], context);
        if (index + 1 < argc) {
            write(" ", context);
        }
    }
    write("\r\n", context);
    return 0;
}

static const cli_command_t g_commands[] = {
    {"help", "show command list", cli_help_handler},
    {"led", "control board led", cli_led_handler},
    {"echo", "echo command arguments", cli_echo_handler},
};

static int cli_help_handler(int argc, char **argv, cli_write_fn_t write, void *context) {
    (void)argc;
    (void)argv;
    cli_dispatch_print_help(g_commands, (int)(sizeof(g_commands) / sizeof(g_commands[0])), write, context);
    return 0;
}

static void cli_process_line(char *line, void *context) {
    char *argv[CLI_MAX_ARGS];
    int argc = 0;

    (void)context;

    if (cli_parser_parse(line, &argc, argv, CLI_MAX_ARGS) != 0) {
        cli_write("error: too many arguments\r\n", NULL);
        return;
    }

    cli_dispatch_execute(g_commands,
                         (int)(sizeof(g_commands) / sizeof(g_commands[0])),
                         argc,
                         argv,
                         cli_write,
                         NULL);
}

int main(void) {
    board_led_init();
    usart_cli_port_init(115200);
    ringbuf_init(&g_cli_rx_buffer, g_cli_rx_storage, sizeof(g_cli_rx_storage));
    cli_input_pump_init(&g_cli_pump,
                        &g_cli_rx_buffer,
                        g_cli_line,
                        sizeof(g_cli_line),
                        cli_process_line,
                        NULL);
    usart_cli_port_write_string("STM32F103 USART CLI ready\r\n");
    usart_cli_port_write_string("Type 'help' for commands\r\n> ");

    while (1) {
        char ch;

        if (usart_cli_port_try_read_char(&ch)) {
            usart_cli_port_write_char(ch);

            if (ch == '\r' || ch == '\n') {
                usart_cli_port_write_string("\r\n");
            }

            if (cli_input_pump_push(&g_cli_pump, ch) != 0) {
                ringbuf_reset(&g_cli_rx_buffer);
                usart_cli_port_write_string("\r\nerror: line too long\r\n> ");
            } else {
                int status = cli_input_pump_process(&g_cli_pump);

                if (status > 0) {
                    usart_cli_port_write_string("> ");
                } else if (status < 0) {
                    ringbuf_reset(&g_cli_rx_buffer);
                    usart_cli_port_write_string("error: line too long\r\n> ");
                }
            }
        }
        delay_ms(1);
    }
}
