
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bme680.h"
#include "i2c.h"
#include "spi.h"

#define AMBIENT_TEMP_GUESS 19.0
#define HEATER_TARGET 300.0

int main(void) {
	
	bme680_t bme680;
	uint8_t mode;
	int i;

	/* 1. Assign functions for interacting with the device */

//	bme680.dev.init   = i2c_init;	
//	bme680.dev.read   = i2c_read;
//	bme680.dev.write  = i2c_write;
//	bme680.dev.deinit = i2c_deinit;

	bme680.dev.init   = spi_init;
	bme680.dev.read   = spi_read;
	bme680.dev.write  = spi_write;
	bme680.dev.deinit = spi_deinit;

	/* 2. set the device mode */
	mode = BME680_MODE_FLOAT | BME680_SPI | BME680_ENABLE_GAS;
	/*     BME680_MODE_INT   | BME680_I2C; */


	/* 3. initialise dev func's, and check device id */
	if (bme680_init(&bme680, mode) != 0) {
		fprintf(stderr, "bme680_init()\n");
		exit(EXIT_FAILURE);
	}

	/* 4. reset device */
	bme680_reset(&bme680);

	/* 5. read calibration parameters from the device and store in memory */
	if (bme680_calibrate(&bme680) != 0) {
		fprintf(stderr, "bme680_calibrate()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* debug */
	bme680_print_calibration(&bme680);

	/* 6. set up device config */
	bme680.cfg.osrs_t = BME680_OVERSAMPLE_X16;
	bme680.cfg.osrs_p = BME680_OVERSAMPLE_X16;
	bme680.cfg.osrs_h = BME680_OVERSAMPLE_X8;
	bme680.cfg.filter = BME680_IIR_COEFF_127;

	/* configuring gas sensor. */
	/* NB: mode |= BME680_ENABLE_GAS; */
	/* there are 10 possible heater setpoints */
	/* they can be thought of as frames that define one single gas-resistance measurement */
	/* they do not carry over or affect each other in any way. */
	for(i=0; i<10; i++) {

		/* calculate a resistance target, based on desired temp, and ambient temp */
		bme680.cfg.res_heat[i] = bme680_calc_target(&bme680, HEATER_TARGET, AMBIENT_TEMP_GUESS);

		/* initial heating current for the setpoint. Could be useful in cold places or short gas_wait durations */
		/* 7-bit word. Each step/lsb is equiv. to 1/8 mA; so max 16 mA */
		/* a value of 20 would be equal to 2.5 mA */
		/* this s.p. field is allowed to be left as 0 if no preload is required. */
		bme680.cfg.idac_heat[i] = BME680_IDAC(100);

		/* define the time between the start of heating and start of resistance sensing in this s.p.*/
		/* Bosch datasheet suggests ~30 - 40ms is usually all that is required to get up to temp. */
		/* ^ probably with a good idac_heat current but doesn't mention it. */
		/* 25 * X4 = 100 ms wait before sampling resistance starts. */
		/* the first value is 6-bit (0...63) with 1 ms step size. */
		bme680.cfg.gas_wait[i] = BME680_GAS_WAIT(25, BME680_GAS_WAIT_X4);
	}

	/* The BME680 does not cycle between setpoints. They have to be manually set. */
	/* After each "run", the setpoint has to be changed. */
	/* the func bme680_write_setpoint_index() will write this field to the dev, without having to reconfigure */
	bme680.setpoint = 0; /* 0 thru 9 */

	/* 7. write config to device */
	if (bme680_configure(&bme680) != 0) {
		fprintf(stderr, "bme680_configure()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* 8. Start forced measurement*. After it finishes, it should remember the previous config. */
	/*           * = bosch speak for one single T, P, H, GasRes measuring cycle. */
	if (bme680_start(&bme680) != 0) {
		fprintf(stderr, "bme680_start()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* 9. poll the meas_status register until all scheduled conversions are done */
	/* this step can be skipped with SPI interrupt (3 wire SPI only. See 5.3.1.1 in datasheet; pg 29) */
	if (bme680_poll(&bme680) != 0) {
		fprintf(stderr, "bme680_poll()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* 10. read the ADC's and perform a conversion */
	if (bme680_read(&bme680) != 0) {
		fprintf(stderr, "bme680_read()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* 11. use data ! */
	if (BME680_IS_FLOAT(bme680.mode)) {
		puts("float mode");
		printf("tfine: %f\n", bme680.fcomp.tfine);
		printf("temp: %f degC\n", bme680.fcomp.temp);
		printf("press: %f Pa\n", bme680.fcomp.press);
		printf("humidity: %f %% RH\n", bme680.fcomp.hum);
		if (BME680_GAS_ENABLED(bme680.mode)) {
			printf("gas resistance: %f Ohm\n", bme680.fcomp.gas_res);
			printf("== for heater target=%.1f and ambient temp=%.1f (degC)\n", HEATER_TARGET, AMBIENT_TEMP_GUESS);
		}
	} else {
		puts("integer mode");
		printf("tfine: %d\n", bme680.icomp.tfine);
		printf("temp: %d (degC * 100)\n", bme680.icomp.temp);
		printf("press: %d Pa\n", bme680.icomp.press);
		printf("humidity: %d (%% RH * 1000)\n", bme680.icomp.hum);
		if (BME680_GAS_ENABLED(bme680.mode)) {
			printf("gas resistance: %d Ohm\n", bme680.icomp.gas_res);
			printf("== for heater target=%.1f and ambient temp=%.1f (degC)\n", HEATER_TARGET, AMBIENT_TEMP_GUESS);
		}
	}

	/* print out heat stability and gas valid flags. should both be 1 !! */
	if (BME680_GAS_ENABLED(bme680.mode)) {
		printf("=== gas_valid_r: %d\n", bme680.gas_valid);
		printf("=== heat_stab_r: %d\n", bme680.heat_stab);
	}

	bme680_deinit(&bme680);

	return 0;
}

