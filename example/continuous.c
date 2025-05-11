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


/* for usleep() */
#define _DEFAULT_SOURCE 


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bme680.h"
#include "i2c.h"
#include "spi.h"

#define AMBIENT_TEMP_GUESS 19.0
#define HEATER_TARGET 300.0

void pretty_print(bme680_t *bme680);

int main(void) {
	
	bme680_t bme680;
	uint8_t mode;
	int counter;

	bme680.dev.init   = spi_init;
	bme680.dev.read   = spi_read;
	bme680.dev.write  = spi_write;
	bme680.dev.deinit = spi_deinit;

	bme680.dev.sleep  = usleep;

	mode = BME680_MODE_FLOAT | BME680_SPI | BME680_ENABLE_GAS;

	if (bme680_init(&bme680, mode) != 0) {
		fprintf(stderr, "bme680_init()\n");
		exit(EXIT_FAILURE);
	}

	bme680_reset(&bme680);

	if (bme680_calibrate(&bme680) != 0) {
		fprintf(stderr, "bme680_calibrate()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	bme680.cfg.osrs_t = BME680_OVERSAMPLE_X16;
	bme680.cfg.osrs_p = BME680_OVERSAMPLE_X16;
	bme680.cfg.osrs_h = BME680_OVERSAMPLE_X8;
	bme680.cfg.filter = BME680_IIR_COEFF_127;

	/* only 1 setpoint */
	bme680.cfg.res_heat[0] = bme680_calc_target(&bme680, HEATER_TARGET, AMBIENT_TEMP_GUESS);
	bme680.cfg.idac_heat[0] = BME680_IDAC(100);
	bme680.cfg.gas_wait[0] = BME680_GAS_WAIT(50, BME680_GAS_WAIT_X16);

	bme680.setpoint = 0;
	
	if (bme680_configure(&bme680) != 0) {
		fprintf(stderr, "bme680_configure()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	counter = 0;

	printf("heater_target=%.1f, ambient_temp=%.1f\n", HEATER_TARGET, AMBIENT_TEMP_GUESS);

	do {
		if (bme680_start(&bme680) != 0) {
			fprintf(stderr, "bme680_start()\n");
			bme680_deinit(&bme680);
			exit(EXIT_FAILURE);
		}

		if (bme680_poll(&bme680) != 0) {
			fprintf(stderr, "bme680_poll()\n");
			bme680_deinit(&bme680);
			exit(EXIT_FAILURE);
		}

		if (bme680_read(&bme680) != 0) {
			fprintf(stderr, "bme680_read()\n");
			bme680_deinit(&bme680);
			exit(EXIT_FAILURE);
		}

		pretty_print(&bme680);

	} while(counter++ < 100);

	bme680_deinit(&bme680);

	return 0;
}

void pretty_print(bme680_t *bme680) {

	double t = bme680->fcomp.temp;
	double p = bme680->fcomp.press / 100.0;
	double h = bme680->fcomp.hum;
	double gr = bme680->fcomp.gas_res;

	uint8_t gv = bme680->gas_valid;
	uint8_t hs = bme680->heat_stab;

	printf("temp=%.2f°C, press=%.2f mbar, hum=%.2f %%RH, gas_res=%.2f gv=%d hs=%d\n",
		       	t, p, h, gr, gv, hs);
}
