#ifndef SPI_H
#define SPI_H
#include "types.h"

void spi_init(u8 mode);
void spi_assert(u8 pin);
void spi_deassert(u8 pin);
u8 spi_exchange(u8 byte);
#endif
