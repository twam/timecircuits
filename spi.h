#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>
#include <stdlib.h>

void spi_init();
void spi_exit();
void spi_transfer(uint16_t *buf, size_t len);

#endif