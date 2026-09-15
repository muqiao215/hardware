#include <stdio.h>

#include "cli_dispatch.h"
#include "cli_parser.h"

static char g_output[128];
static int g_output_index = 0;

static void reset_output(void) {
    int index;

    g_output_index = 0;
    for (index = 0; index < (int)sizeof(g_output); index++) {
        g_output[index] = '\0';
    }
}

static void append_output(const char *text, void *context) {
    (void)context;

    while (*text != '\0' && g_output_index + 1 < (int)sizeof(g_output)) {
        g_output[g_output_index++] = *text++;
    }
}

static int cmd_echo(int argc, char **argv, cli_write_fn_t write, void *context) {
    int index;

    for (index = 1; index < argc; index++) {
        write(argv[index], context);
        if (index + 1 < argc) {
            write(" ", context);
        }
    }
    return 0;
}

static int cmd_help(int argc, char **argv, cli_write_fn_t write, void *context);

static const cli_command_t g_commands[] = {
    {"echo", "echo args", cmd_echo},
    {"help", "list commands", cmd_help},
};

static int cmd_help(int argc, char **argv, cli_write_fn_t write, void *context) {
    (void)argc;
    (void)argv;
    cli_dispatch_print_help(g_commands, 2, write, context);
    return 0;
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

int main(void) {
    char line1[] = "echo hello stm32";
    char line2[] = "help";
    char line3[] = "unknown";
    char line4[] = "   echo   spaced   args   ";
    char line5[] = "";
    char line6[] = "echo 1 2 3 4";
    char *argv[4];
    int argc = 0;

    if (cli_parser_parse(line1, &argc, argv, 4) != 0) {
        fprintf(stderr, "cli_parser_parse failed for echo case\n");
        return 1;
    }
    if (argc != 3 || !strings_equal(argv[0], "echo") || !strings_equal(argv[1], "hello") ||
        !strings_equal(argv[2], "stm32")) {
        fprintf(stderr, "cli_parser_parse returned unexpected argv data\n");
        return 1;
    }

    reset_output();
    if (cli_dispatch_execute(g_commands, 2, argc, argv, append_output, NULL) != 0) {
        fprintf(stderr, "cli_dispatch_execute failed for echo case\n");
        return 1;
    }
    if (!strings_equal(g_output, "hello stm32")) {
        fprintf(stderr, "unexpected echo output: %s\n", g_output);
        return 1;
    }

    if (cli_parser_parse(line2, &argc, argv, 4) != 0) {
        fprintf(stderr, "cli_parser_parse failed for help case\n");
        return 1;
    }
    reset_output();
    if (cli_dispatch_execute(g_commands, 2, argc, argv, append_output, NULL) != 0) {
        fprintf(stderr, "cli_dispatch_execute failed for help case\n");
        return 1;
    }
    if (!strings_equal(g_output, "echo - echo args\r\nhelp - list commands\r\n")) {
        fprintf(stderr, "unexpected help output: %s\n", g_output);
        return 1;
    }

    if (cli_parser_parse(line3, &argc, argv, 4) != 0) {
        fprintf(stderr, "cli_parser_parse failed for unknown case\n");
        return 1;
    }
    reset_output();
    if (cli_dispatch_execute(g_commands, 2, argc, argv, append_output, NULL) == 0) {
        fprintf(stderr, "unknown command unexpectedly succeeded\n");
        return 1;
    }
    if (!strings_equal(g_output, "error: unknown command\r\n")) {
        fprintf(stderr, "unexpected unknown output: %s\n", g_output);
        return 1;
    }

    if (cli_parser_parse(line4, &argc, argv, 4) != 0) {
        fprintf(stderr, "cli_parser_parse failed for spaced case\n");
        return 1;
    }
    if (argc != 3 || !strings_equal(argv[0], "echo") || !strings_equal(argv[1], "spaced") ||
        !strings_equal(argv[2], "args")) {
        fprintf(stderr, "cli_parser_parse returned unexpected argv data for spaced case\n");
        return 1;
    }

    if (cli_parser_parse(line5, &argc, argv, 4) != 0) {
        fprintf(stderr, "cli_parser_parse failed for empty case\n");
        return 1;
    }
    if (argc != 0) {
        fprintf(stderr, "empty line should produce argc=0\n");
        return 1;
    }

    if (cli_parser_parse(line6, &argc, argv, 4) == 0) {
        fprintf(stderr, "too many args case unexpectedly succeeded\n");
        return 1;
    }

    printf("host cli tests passed\n");
    return 0;
}
