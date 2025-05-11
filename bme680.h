/*
 * This file is part of bme680.
 *
 * Copyright (C) 2025 William Clark
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef BME680_H
#define BME680_H

#include <stdint.h>

/* REGISTERS */

/* confuses me too ! */
/* SPI page number 0 => 0x00 to 0x7F */
/* SPI page number 1 => 0x80 to 0xFF */
#define REG_SPI_PAGE(v) ((v > 0x7F) ? 0 : 1)
#define REG_SPI_PAGE_MAP(v) (v == 0 ? "LOW" : "HIGH")

#define REG_STATUS      0x73
#define REG_RESET       0xE0
#define REG_ID          0xD0
#define REG_CONFIG      0x75
#define REG_CTRL_MEAS   0x74
#define REG_CTRL_HUM    0x72

#define REG_CTRL_GAS_1  0x71
#define REG_CTRL_GAS_0  0x70

#define REG_GAS_WAIT_9  0x6D
#define REG_GAS_WAIT_8  0x6C
#define REG_GAS_WAIT_7  0x6B
#define REG_GAS_WAIT_6  0x6A
#define REG_GAS_WAIT_5  0x69
#define REG_GAS_WAIT_4  0x68
#define REG_GAS_WAIT_3  0x67
#define REG_GAS_WAIT_2  0x66
#define REG_GAS_WAIT_1  0x65
#define REG_GAS_WAIT_0  0x64

#define REG_RES_HEAT_9  0x63
#define REG_RES_HEAT_8  0x62
#define REG_RES_HEAT_7  0x61
#define REG_RES_HEAT_6  0x60
#define REG_RES_HEAT_5  0x5F
#define REG_RES_HEAT_4  0x5E
#define REG_RES_HEAT_3  0x5D
#define REG_RES_HEAT_2  0x5C
#define REG_RES_HEAT_1  0x5B
#define REG_RES_HEAT_0  0x5A

#define REG_IDAC_HEAT_9 0x59
#define REG_IDAC_HEAT_8 0x58
#define REG_IDAC_HEAT_7 0x57
#define REG_IDAC_HEAT_6 0x56
#define REG_IDAC_HEAT_5 0x55
#define REG_IDAC_HEAT_4 0x54
#define REG_IDAC_HEAT_3 0x53
#define REG_IDAC_HEAT_2 0x52
#define REG_IDAC_HEAT_1 0x51
#define REG_IDAC_HEAT_0 0x50

#define REG_GAS_R_LSB   0x2B
#define REG_GAS_R_MSB   0x2A

#define REG_HUM_LSB     0x26
#define REG_HUM_MSB     0x25

#define REG_TEMP_XLSB   0x24
#define REG_TEMP_LSB    0x23
#define REG_TEMP_MSB    0x22

#define REG_PRESS_XLSB  0x21
#define REG_PRESS_LSB   0x20
#define REG_PRESS_MSB   0x1F

#define REG_MEAS_STATUS 0x1D

/* END OF REGISTERS */

#define BME680_IS_SPI(m)      ((m & 1) == 1)
#define BME680_IS_FLOAT(m)    (((m >> 1) & 1) == 0)
#define BME680_GAS_ENABLED(m) (((m >> 2) & 1) == 1) 

#define BME680_IDAC(c) (c << 1)
#define BME680_GAS_WAIT(val, scal) ((uint8_t)(((scal & 3) << 6) | (val & 63)))

/* connection modes */
#define BME680_SPI   1
#define BME680_I2C   0

/* calculation modes; int or float calc */
#define BME680_MODE_INT   2
#define BME680_MODE_FLOAT 0

/* to enable gas conversion OR this into mode byte */
#define BME680_ENABLE_GAS 4

/* config values */
#define BME680_OVERSAMPLE_X1  1
#define BME680_OVERSAMPLE_X2  2
#define BME680_OVERSAMPLE_X4  3
#define BME680_OVERSAMPLE_X8  4
#define BME680_OVERSAMPLE_X16 5

/* IIR filter */
#define BME680_IIR_COEFF_0    0
#define BME680_IIR_COEFF_1    1
#define BME680_IIR_COEFF_3    2
#define BME680_IIR_COEFF_7    3
#define BME680_IIR_COEFF_15   4
#define BME680_IIR_COEFF_31   5
#define BME680_IIR_COEFF_63   6
#define BME680_IIR_COEFF_127  7

/* gas related values */
#define BME680_GAS_WAIT_X1    0
#define BME680_GAS_WAIT_X4    1
#define BME680_GAS_WAIT_X16   2
#define BME680_GAS_WAIT_X64   3


/* user supplied spi/i2c functions */
struct bme680_dev {
	int (*init)   (void);
	int (*read)   (uint8_t reg, uint8_t *dst, uint32_t size);
	int (*write)  (uint8_t reg, uint8_t value);
	int (*sleep)  (uint32_t dur_us);
	int (*deinit) (void);
};

struct bme680_cal {
	/* temp calibration */
	uint16_t par_t1;
	int16_t  par_t2;
	int8_t   par_t3;

	/* press calibration */
	uint16_t par_p1;
	int16_t  par_p2;
	int8_t   par_p3; 
	int16_t  par_p4;
	int16_t  par_p5;
	int8_t   par_p6; 
	int8_t   par_p7; 
	int16_t  par_p8; 
	int16_t  par_p9;
	uint8_t  par_p10;

	/* humidity calibration */
	uint16_t par_h1; 
	uint16_t par_h2;
	int8_t   par_h3;
	int8_t   par_h4;
	int8_t   par_h5;
	uint8_t  par_h6;
	int8_t   par_h7;

	/* heater + gas */
	uint16_t par_g1;
	uint16_t par_g2;
	uint16_t par_g3;
	uint16_t range_switching_error;
	uint8_t  res_heat_range;
	int8_t   res_heat_val;
};

struct bme680_config {
	uint8_t osrs_t;
	uint8_t osrs_p;
	uint8_t osrs_h;
	uint8_t filter;
	uint8_t idac_heat[10];
	uint8_t res_heat[10];
	uint8_t gas_wait[10];
	uint8_t meas; /* required because you can't read back ctrl regs */
};

struct bme680_comp_float {
	double tfine;
	double temp;
	double press;
	double hum;
	double gas_res;
};

struct bme680_comp_int {
	int32_t tfine;
	int32_t temp;
	int32_t press;
	int32_t hum;
	int32_t gas_res;
};

struct bme680_adc {
	uint32_t temp;
	uint32_t press;
	uint32_t hum;
	uint32_t gas;
	uint32_t gas_range;

};

struct bme680 {
	struct bme680_comp_float fcomp;
	struct bme680_comp_int   icomp;
	struct bme680_cal cal;
	struct bme680_dev dev;
	struct bme680_config cfg;
	struct bme680_adc adc;
	uint8_t mode;
	uint8_t setpoint;
	uint8_t spi_page; 
	uint8_t gas_valid;
	uint8_t heat_stab;
};

typedef struct bme680 bme680_t;

int bme680_init(bme680_t *bme680, uint8_t mode);
int bme680_deinit(bme680_t *bme680);
int bme680_reset(bme680_t *bme680);
int bme680_calibrate(bme680_t *bme680);
int bme680_configure(bme680_t *bme680);
int bme680_start(bme680_t *bme680);
int bme680_poll(bme680_t *bme680);
int bme680_read(bme680_t *bme680);
int bme680_write_setpoint_index(bme680_t *bme680);
int bme680_read_setpoint_index(bme680_t *bme680, uint8_t *index);

uint8_t bme680_calc_target(bme680_t *bme680, double target, double ambient);

#endif
