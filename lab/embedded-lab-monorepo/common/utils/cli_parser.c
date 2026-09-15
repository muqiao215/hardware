#include "cli_parser.h"

static int cli_parser_is_space(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

int cli_parser_parse(char *line, int *argc, char **argv, int max_args) {
    int count = 0;

    while (*line != '\0') {
        while (cli_parser_is_space(*line)) {
            *line++ = '\0';
        }

        if (*line == '\0') {
            break;
        }

        if (count >= max_args) {
            return -1;
        }

        argv[count++] = line;

        while (*line != '\0' && !cli_parser_is_space(*line)) {
            line++;
        }
    }

    *argc = count;
    return 0;
}
