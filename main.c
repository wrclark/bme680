/* usleep() */
#define _DEFAULT_SOURCE

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

	/* 1. Assign functions for interacting with the device */
	bme680.dev.init = linux_i2c_init;	
	bme680.dev.read = linux_i2c_read;
	bme680.dev.write = linux_i2c_write;
	bme680.dev.deinit = linux_i2c_deinit;
	bme680.dev.xfer = NULL;

	/* 2. set the device mode */
	bme680.mode = BME680_MODE_FLOAT | BME680_I2C;
	/*            BME680_MODE_INT   | BME680_SPI; */


	/* 3. initialise device, and check its id */
	if (bme680_init(&bme680) != 0) {
		fprintf(stderr, "bme680_init()\n");
		exit(EXIT_FAILURE);
	}

	/* 4. read calibration parameters from the device and store in memory */
	if (bme680_calibrate(&bme680) != 0) {
		fprintf(stderr, "bme680_calibrate()\n");
		bme680.dev.deinit();
		exit(EXIT_FAILURE);
	}

	/* 5. set up device config */
	bme680.cfg.osrs_t = BME680_OVERSAMPLE_16X;
	bme680.cfg.osrs_p = BME680_OVERSAMPLE_8X;
	bme680.cfg.osrs_h = BME680_OVERSAMPLE_4X;




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
	puts("linux_i2c_read");
	if (i2c_read_reg(i2c_dev_fd, reg, (uint8_t)size, dst) != 0) {
		printf("panic read: %d %p %d\n", reg, dst, size);
		return 1;
	}
	return 0;
}

int linux_i2c_write (uint8_t reg, uint8_t *src, uint32_t size) {
	puts("linux_i2c_write");
	if (i2c_write_reg(i2c_dev_fd, reg, *src) != 0) {
		puts("panic!");
		return 1;
	}
	return 0;
}

int linux_i2c_deinit (void) {
	close(i2c_dev_fd);
	return 0;
}
