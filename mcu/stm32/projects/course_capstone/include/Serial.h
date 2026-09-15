#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

void Serial_Init(uint32_t baud);
int Serial_ReadLine(char *out, uint16_t out_size);
void Serial_SendByte(uint8_t Byte);
void Serial_SendString(const char *String);
void Serial_Printf(const char *format, ...);

#endif

