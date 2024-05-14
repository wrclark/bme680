#define _DEFAULT_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "bme680.h"
#include "i2c.h"

#define AMBIENT_TEMP_GUESS 19.0
#define HEATER_TARGET 300.0


int main(void) {
	
	bme680_t bme680;
	uint8_t mode;
	double temperature = AMBIENT_TEMP_GUESS;
	time_t curr_time;
	char date[100];

	bme680.dev.init   = i2c_init;	
	bme680.dev.read   = i2c_read;
	bme680.dev.write  = i2c_write;
	bme680.dev.deinit = i2c_deinit;
	bme680.dev.sleep  = usleep;

	mode = BME680_MODE_FLOAT | BME680_I2C | BME680_ENABLE_GAS;


	if (bme680_init(&bme680, mode) != 0) {
		fprintf(stderr, "bme680_init()\n");
		exit(EXIT_FAILURE);
	}

START:
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

	bme680.cfg.res_heat[0] = bme680_calc_target(&bme680, HEATER_TARGET, temperature);
	bme680.cfg.idac_heat[0] = BME680_IDAC(100);
	bme680.cfg.gas_wait[0] = BME680_GAS_WAIT(25, BME680_GAS_WAIT_X4);
	
	bme680.setpoint = 0;

	if (bme680_configure(&bme680) != 0) {
		fprintf(stderr, "bme680_configure()\n");
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

	curr_time = time(NULL);
	strftime(date, 100, "%FT%T%z", localtime(&curr_time));
	printf("%s %g %g %g %g %d\n",
			date,
			bme680.fcomp.temp,
			bme680.fcomp.press,
			bme680.fcomp.hum,
			bme680.fcomp.gas_res,
			!!bme680.heat_stab);

	temperature = bme680.fcomp.temp;

	// 60 secs
	usleep(1000*1000*60);
	goto START;

	bme680_deinit(&bme680);

	return 0;
}

