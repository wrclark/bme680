#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define I2C_OK  0
#define I2C_ERR 1

int i2c_init      (const char *dev, uint8_t addr);
int i2c_read_reg  (int fd, uint8_t reg, uint8_t *dst, uint32_t size);
int i2c_write_reg (int fd, uint8_t reg, uint8_t value);

#endif
