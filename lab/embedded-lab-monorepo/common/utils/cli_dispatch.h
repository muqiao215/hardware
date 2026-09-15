#ifndef CLI_DISPATCH_H
#define CLI_DISPATCH_H

typedef void (*cli_write_fn_t)(const char *text, void *context);
typedef int (*cli_command_handler_t)(int argc, char **argv, cli_write_fn_t write, void *context);

typedef struct {
    const char *name;
    const char *help;
    cli_command_handler_t handler;
} cli_command_t;

int cli_dispatch_execute(const cli_command_t *commands,
                         int command_count,
                         int argc,
                         char **argv,
                         cli_write_fn_t write,
                         void *context);

void cli_dispatch_print_help(const cli_command_t *commands,
                             int command_count,
                             cli_write_fn_t write,
                             void *context);

#endif
