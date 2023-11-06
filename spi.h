#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#define SPI_OK  0
#define SPI_ERR 1

int spi_init   (void);
int spi_read   (uint8_t reg, uint8_t *dst, uint32_t size);
int spi_write  (uint8_t reg, uint8_t value);
int spi_deinit (void);

#endif
