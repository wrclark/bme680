
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bme680.h"
#include "i2c.h"
#include "spi.h"

#define AMBIENT_TEMP_GUESS 19.0

/* heater prob won't settle */
#define HEATER_TARGET_CLEAN_SENSOR 380.0

/* for the "real" measurement */
#define HEATER_TARGET 300.0

void pretty_print(bme680_t *bme680);

int main(void) {
	
	bme680_t bme680;
	uint8_t mode;
	int i;

	bme680.dev.init   = spi_init;
	bme680.dev.read   = spi_read;
	bme680.dev.write  = spi_write;
	bme680.dev.deinit = spi_deinit;

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

	for(i=0; i<10; i++) {
		/* extra heat to clean the sensor surface. might work ! */
		if (i < 6) {
			bme680.cfg.res_heat[i] = bme680_calc_target(&bme680, HEATER_TARGET_CLEAN_SENSOR, AMBIENT_TEMP_GUESS);
			bme680.cfg.idac_heat[i] = BME680_IDAC(100); //  12.5 mA  
			bme680.cfg.gas_wait[i] = BME680_GAS_WAIT(50, BME680_GAS_WAIT_X4);
		} else {
			bme680.cfg.res_heat[i] = bme680_calc_target(&bme680, HEATER_TARGET, AMBIENT_TEMP_GUESS);
			bme680.cfg.idac_heat[i] = BME680_IDAC(50);
			bme680.cfg.gas_wait[i] = BME680_GAS_WAIT(50, BME680_GAS_WAIT_X4);
		}
	}

	bme680.setpoint = 0;
	
	if (bme680_configure(&bme680) != 0) {
		fprintf(stderr, "bme680_configure()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}


	printf("heater_target=%.1f, ambient_temp=%.1f\n", HEATER_TARGET, AMBIENT_TEMP_GUESS);
	printf("first few setpoints have heater_target=%1.f\n", HEATER_TARGET_CLEAN_SENSOR);

	for(i=0; i<10; i++) {

		bme680.setpoint = i;

		if (bme680_write_setpoint_index(&bme680) != 0) {
			fprintf(stderr, "bme680_write_setpoint()\n");
			bme680_deinit(&bme680);
			exit(EXIT_FAILURE);
		}

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
	}

	bme680_deinit(&bme680);

	return 0;
}

void pretty_print(bme680_t *bme680) {
	uint8_t dev_sp;

	bme680_read_setpoint_index(bme680, &dev_sp);

	double t = bme680->fcomp.temp;
	double p = bme680->fcomp.press / 100.0;
	double h = bme680->fcomp.hum;
	double gr = bme680->fcomp.gas_res;

	int gv = bme680->gas_valid;
	int hs = bme680->heat_stab;

	printf("[Setpoint %d] temp=%.2f°C, press=%.2f mbar, hum=%.2f %%RH, gas_res=%.2f gv=%d hs=%d\n",
			dev_sp, t, p, h, gr, gv, hs);
}
