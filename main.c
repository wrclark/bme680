
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bme680.h"
#include "i2c.h"

#define DEVICE "/dev/i2c-1"
#define ADDRESS 0x77

int i2c_dev_fd;

int linux_i2c_init   (void);
int linux_i2c_read   (uint8_t reg, uint8_t *dst, uint32_t size);
int linux_i2c_write  (uint8_t reg, uint8_t *src, uint32_t size);
int linux_i2c_deinit (void);

int main(){
	
	bme680_t bme680;
	uint8_t mode;

	/* 1. Assign functions for interacting with the device */
	bme680.dev.init   = linux_i2c_init;	
	bme680.dev.read   = linux_i2c_read;
	bme680.dev.write  = linux_i2c_write;
	bme680.dev.deinit = linux_i2c_deinit;
	bme680.dev.xfer = NULL;

	/* 2. set the device mode */
	mode = BME680_MODE_FLOAT | BME680_I2C;
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
	bme680.cfg.osrs_t = BME680_OVERSAMPLE_16X;
	bme680.cfg.osrs_p = BME680_OVERSAMPLE_16X;
	bme680.cfg.osrs_h = BME680_OVERSAMPLE_8X;
	bme680.cfg.filter = BME680_IIR_COEFF_63;


	/* 7. write config to device and set off conversion */
	if (bme680_start(&bme680) != 0) {
		fprintf(stderr, "bme680_start()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	/* 8. poll the meas_status register until all scheduled conversions are done */
	if (bme680_poll(&bme680) != 0) {
		fprintf(stderr, "bme680_poll()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}


	/* 9. read the ADC's and perform a conversion */
	if (bme680_read(&bme680) != 0) {
		fprintf(stderr, "bme680_read()\n");
		bme680_deinit(&bme680);
		exit(EXIT_FAILURE);
	}

	printf("adc_temp: %d\n", bme680.adc.temp);
	printf("adc_press: %d\n", bme680.adc.press);
	printf("adc_hum: %d\n", bme680.adc.hum);

	/* 10. use data ! */

	if (BME680_IS_FLOAT(bme680.mode)) {
		puts("float mode");
		printf("tfine: %f\n", bme680.fcomp.tfine);
		printf("temp: %f degC\n", bme680.fcomp.temp);
		printf("press: %f Pa\n", bme680.fcomp.press);
		printf("humidity: %f %% RH\n", bme680.fcomp.hum);
	} else {
		puts("integer mode");
		printf("tfine: %d\n", bme680.icomp.tfine);
		printf("temp: %d\n", bme680.icomp.temp);
		printf("press: %d Pa\n", bme680.icomp.press);
		printf("humidity: %d %% RH\n", bme680.icomp.hum);
	}

	bme680_deinit(&bme680);

	return 0;
}

// STUBS

int linux_i2c_init (void) {
	puts("linux_i2c_init");
	int ret = i2c_init(DEVICE, ADDRESS);
	if (ret > 0) {
		i2c_dev_fd = ret;
		return 0;
	}

	return 1;
}

int linux_i2c_read (uint8_t reg, uint8_t *dst, uint32_t size) {

	int i;
	int ret;

	printf("linux_i2c_read: %X (%d) [", reg, size);

	ret = i2c_read_reg(i2c_dev_fd, reg, (uint8_t)size, dst);
	if (ret != 0) {
		return -1;
	}

	for(i=0; i<size; i++) {
		printf("%X", dst[i]);
		if (i < (size - 1)) {
			printf(",");
		}
	}

	printf("]\n");
	
	return 0;
}

int linux_i2c_write (uint8_t reg, uint8_t *src, uint32_t size) {

	int i;

	printf("linux_i2c_write: %X (%d) [", reg, size);
	for(i=0; i<size; i++) {
		printf("%X", src[i]);
		if (i < (size - 1)) {
			printf(",");
		}
	}
	printf("]\n");

	if (i2c_write_reg(i2c_dev_fd, reg, *src) != 0) {
		return 1;
	}
	return 0;
}

int linux_i2c_deinit (void) {
	puts("linux_i2c_deinit");
	close(i2c_dev_fd);
	return 0;
}
