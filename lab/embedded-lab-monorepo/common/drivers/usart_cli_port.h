#ifndef USART_CLI_PORT_H
#define USART_CLI_PORT_H

#include <stdint.h>

void usart_cli_port_init(uint32_t baud_rate);
int usart_cli_port_try_read_char(char *ch);
void usart_cli_port_write_char(char ch);
void usart_cli_port_write_string(const char *text);

#endif
