#include "cli_dispatch.h"

#include <stddef.h>

int cli_dispatch_execute(const cli_command_t *commands,
                         int command_count,
                         int argc,
                         char **argv,
                         cli_write_fn_t write,
                         void *context) {
    int index;

    if (argc == 0) {
        return 0;
    }

    for (index = 0; index < command_count; index++) {
        const char *lhs = commands[index].name;
        const char *rhs = argv[0];

        while (*lhs != '\0' && *rhs != '\0' && *lhs == *rhs) {
            lhs++;
            rhs++;
        }

        if (*lhs == '\0' && *rhs == '\0') {
            return commands[index].handler(argc, argv, write, context);
        }
    }

    if (write != NULL) {
        write("error: unknown command\r\n", context);
    }
    return -1;
}

void cli_dispatch_print_help(const cli_command_t *commands,
                             int command_count,
                             cli_write_fn_t write,
                             void *context) {
    int index;

    if (write == NULL) {
        return;
    }

    for (index = 0; index < command_count; index++) {
        write(commands[index].name, context);
        write(" - ", context);
        write(commands[index].help, context);
        write("\r\n", context);
    }
}
