#pragma once

#include <stdint.h>

int i2c_init(const char *, uint8_t);
int i2c_read_reg(int, uint8_t, uint8_t, uint8_t *);
uint8_t i2c_read_reg2(uint8_t, uint8_t);
int i2c_write_reg(int, uint8_t, uint8_t);

