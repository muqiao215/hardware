#ifndef SPI_BUS_H
#define SPI_BUS_H

#include <stdint.h>

void spi_bus_init(void);
uint8_t spi_bus_transfer(uint8_t value);
void spi_bus_select(void);
void spi_bus_deselect(void);

#endif
