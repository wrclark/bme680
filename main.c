
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bme680.h"
#include "i2c.h"

#define DEVICE "/dev/i2c-1"
#define ADDRESS 0x77

#define AMBIENT_TEMP_GUESS 25.0
#define HEATER_TARGET 320.0

int i2c_dev_fd;

int linux_i2c_init   (void);
int linux_i2c_read   (uint8_t reg, uint8_t *dst, uint32_t size);
int linux_i2c_write  (uint8_t reg, uint8_t value);
int linux_i2c_deinit (void);

int main(void) {
	
	bme680_t bme680;
	uint8_t mode;
	int i;

	/* 1. Assign functions for interacting with the device */
	bme680.dev.init   = linux_i2c_init;	
	bme680.dev.read   = linux_i2c_read;
	bme680.dev.write  = linux_i2c_write;
	bme680.dev.deinit = linux_i2c_deinit;

	/* 2. set the device mode */
	mode = BME680_MODE_FLOAT | BME680_I2C | BME680_ENABLE_GAS;
	/*     BME680_MODE_INT   | BME680_SPI; */


	/* 3. initialise device, and check its id */
	if (bme680_init(&bme680, mode) != 0) {
		fprintf(stderr, "bme680_init()\n");
		exit(EXIT_FAILURE);
	}

	/* 4. reset */
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
	/* they can be though of as frames that define one single gas-resistance measurement */
	/* they do not carry over or affect eachother in any way. */
	for(i=0; i<10; i++) {

		/* calculate a resistance target, based on desired temp, and ambient temp */
		bme680.cfg.res_heat[i] = bme680_calc_target(&bme680, HEATER_TARGET, AMBIENT_TEMP_GUESS);

		/* initial heating current for the setpoint. Could be useful in cold places.. */
		/* 7-bit word. Each step/lsb is equiv. to 1/8 mA; so max 16 mA */
		/* this s.p. field is allowed to be left as 0 if no preload is required. */
		bme680.cfg.idac_heat[i] = BME680_IDAC(20);

		/* define the time between the start of heating and start of resistance sensing in this s.p.*/
		/* Bosch datasheet suggests ~30 - 40ms is usually all that is required to get up to temp. */
		/* 60 * 16 = 960 ms wait before sampling resistance starts. */
		/* the first value is 6-bit (0...64) with 1 ms step size. */
		bme680.cfg.gas_wait[i] = BME680_GAS_WAIT(60, BME680_GAS_WAIT_X16);
	}

	/* The BME680 does not cycle between setpoints. They have to be manually set. */
	/* After each "run", the setpoint has to be changed. */
	bme680.cfg.setpoint = 0; /* 0 thru 9 */

	/* 7. write config to device */
	if (bme680_configure(&bme680) != 0) {
		fprintf(stderr, "bme680_configure()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* 8. Start forced measurement. After it finishes, it should remember the previous config. */
	if (bme680_start(&bme680) != 0) {
		fprintf(stderr, "bme680_start()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* 9. poll the meas_status register until all scheduled conversions are done */
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

	bme680_deinit(&bme680);

	return 0;
}


// STUBS

int linux_i2c_init (void) {

	int ret;
	puts("i2c_init");

	if ((ret = i2c_init(DEVICE, ADDRESS)) > 0) {
		i2c_dev_fd = ret;
		return 0;
	}

	return 1;
}

int linux_i2c_read (uint8_t reg, uint8_t *dst, uint32_t size) {

	uint32_t i;

	printf("i2c_read: %.2X (%d) [", reg, size);

	if (i2c_read_reg(i2c_dev_fd, reg, dst, size) != I2C_OK) {
		return 1;
	}

	for(i=0; i<size; i++) {

		printf("%.2X", dst[i]);

		if (i < (size - 1)) {
			printf(", ");
		}
	}

	printf("]\n");
	
	return 0;
}

int linux_i2c_write (uint8_t reg, uint8_t value) {

	printf("i2c_write: %.2X [%.2X]\n", reg, value);

	if (i2c_write_reg(i2c_dev_fd, reg, value) != I2C_OK) {
		return 1;
	}

	return 0;
}

int linux_i2c_deinit (void) {

	puts("i2c_deinit");
	close(i2c_dev_fd);

	return 0;
}
