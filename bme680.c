#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>

#include "bme680.h"
#include "registers.h"

static int write_spi_page(bme680_t *bme680, uint8_t page_value);

static void calc_temp_comp(bme680_t *bme680);
static void calc_press_comp(bme680_t *bme680);
static void calc_hum_comp(bme680_t *bme680);


static int write_dev(bme680_t *bme680, uint8_t reg, uint8_t *src, uint32_t size) {
	uint8_t tmp;

	if (BME680_IS_SPI(bme680->mode)) {
		if ((tmp = REG_SPI_PAGE(reg)) != bme680->spi_page) {
			(void) write_spi_page(bme680, tmp);
			bme680->spi_page = tmp;
		}
	}

	return bme680->dev.write(reg, src, size);
}

static int write_spi_page(bme680_t *bme680, uint8_t page_value) {
	uint8_t status_byte = (!!page_value) << 4; 
	
	return bme680->dev.write(REG_STATUS, &status_byte, 1);
}

static int read_dev(bme680_t *bme680, uint8_t reg, uint8_t *dst, uint32_t size) {

	uint8_t tmp;

	/* Might have to change the SPI page for this reg if using SPI */
	if (BME680_IS_SPI(bme680->mode)) {
		if ((tmp = REG_SPI_PAGE(reg)) != bme680->spi_page) {
			(void) write_spi_page(bme680, tmp);
			bme680->spi_page = tmp;
		}
	}

	/* some registers, like id and reset, have diff addr in i2c/spi */
	return bme680->dev.read(reg, dst, size);
}

int bme680_init(bme680_t *bme680, uint8_t mode) {

	uint8_t car;
	uint8_t id_reg;

	bme680->mode = mode;

	if (bme680->dev.init() != 0) {
		return 1;
	}

	/* id reg depends on spi or i2c */
	id_reg = BME680_IS_SPI(mode) ? 0x50 : REG_ID;

	if (read_dev(bme680, id_reg, &car, 1) != 0) {
		return 1;
	}

	if (car != 0x61) {
		return 1;
	}

	return 0;
}

/* for completion */
int bme680_deinit(bme680_t *bme680) {
	if (bme680->dev.deinit) {
		bme680->dev.deinit();
	}

	return 0;
}

int bme680_reset(bme680_t *bme680) {
	uint8_t reg = BME680_IS_SPI(bme680->mode) ? 0x60 : REG_RESET;
	uint8_t magic = 0xB6;
	int ret;
	
	ret = write_dev(bme680, reg, &magic, 1); 
	usleep(5000); /* sleep for 5 ms */
	return ret;
}


/* configure device and trigger forced mode reading on device */
int bme680_start(bme680_t *bme680) {

	uint8_t meas, hum, filter, err;
	meas = hum = filter = err = 0;

	/* ctrl_meas. the last 0 is ticked on to enable forced mode,
	 * but the config has to be written fist. strange behaviour.
	 */
	meas   = (bme680->cfg.osrs_t) << 5 | (bme680->cfg.osrs_p) << 2; 
	hum    =  bme680->cfg.osrs_h;
	filter = (bme680->cfg.filter) << 2;

	err |= write_dev(bme680, REG_CTRL_MEAS, &meas, 1);
	err |= write_dev(bme680, REG_CTRL_HUM,  &hum,  1);
	err |= write_dev(bme680, REG_CONFIG,  &filter, 1);

	// TODO: gas stuff
	
	/* Now, re-send `meas' but LSb set to 1 to enable a forced conversion */
	meas |= 1;
	err  |= write_dev(bme680, REG_CTRL_MEAS, &meas, 1);

	return err;
}


/* blocks until the device claims all scheduled conversions are done. 
 * don't call out of order.
 */

#define BME680_MAX_POLL_ATTEMPTS 100
int bme680_poll(bme680_t *bme680) {

	uint8_t meas_status = 0;
	int err = 0;
	int attempts = 0;

	do {
		usleep(2000);
		err |= read_dev(bme680, REG_EAS_STATUS, &meas_status, 1);
	} while (((meas_status >> 5) & 1) && attempts++ < BME680_MAX_POLL_ATTEMPTS && !err);

	if (attempts == BME680_MAX_POLL_ATTEMPTS) {
		err = 1;
	}

	return err;
}

/* assume start'd and poll'd */
int bme680_read(bme680_t *bme680) {

	/* begin by reading ADCs */
	uint8_t buffer[3] = {0, 0 ,0};
	int err = 0;

	err |= read_dev(bme680, 0x22, buffer, 3);
	bme680->adc.temp = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
	
	err |= read_dev(bme680, 0x1F, buffer, 3);
	bme680->adc.press = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);

	err |= read_dev(bme680, 0x25, buffer, 2);
	bme680->adc.hum = (buffer[0] << 8) | buffer[1];

	/* adc readings are only 20-bit when the IIR filter is enabled.
	 * otherwise, it depends on the oversample settings.
	 * note: humidity is not IIR filtered, and always 16-bit.
	 * IIR filter on (any level) -> 20-bit
	 * IIR filter off -> 16 + (osrs_x - 1) bits. adc >> (osrs_p -1);?
	 * */
	if (bme680->cfg.filter == BME680_IIR_COEFF_0) {
		bme680->adc.temp  >>= (bme680->cfg.osrs_t - 1);
		bme680->adc.press >>= (bme680->cfg.osrs_p - 1);
	}

	/* read/convert in order ..*/
	calc_temp_comp(bme680);
	calc_press_comp(bme680);
	calc_hum_comp(bme680);

	return err;
}

/********************************************************************/

static double const_array1[16] = {
	1, 1, 1, 1, 1, 0.99, 1, 0.992, 1,
       	1, 0.998, 0.995, 1, 0.99, 1, 1
};

static double const_array2[16] = {
	8000000, 4000000, 2000000, 1000000, 499500.4995, 248262.1648,
       	125000, 63004.03226, 31281.28128, 15625, 7812.5, 3906.25, 
	1953.125, 976.5625, 488.28125, 244.140625
};

static int const_array1_int[16] = {
	2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 
	2126008810, 2147483647, 2130303777, 2147483647, 2147483647,
	2143188679, 2136746228, 2147483647, 2126008810, 2147483647,
	2147483647
};

static int const_array2_int[16] = {
	4096000000, 2048000000, 1024000000, 512000000, 255744255,
	127110228, 64000000, 32258064, 16016016, 8000000, 4000000,
	2000000, 1000000, 500000, 250000, 125000
};

/********************************************************************/
/********************************************************************/
static void calc_temp_comp_1 (bme680_t *bme680) {

	double var1, var2, temp_comp;

	var1 = (((double)bme680->adc.temp / 16384.0) - ((double)bme680->cal.par_t1 / 1024.0)) * 
		(double)bme680->cal.par_t2;
	var2 = ((((double)bme680->adc.temp / 131072.0) - ((double)bme680->cal.par_t1 / 8192.0)) *
		(((double)bme680->adc.temp / 131072.0) - ((double)bme680->cal.par_t1 / 8192.0))) *
			((double)bme680->cal.par_t3 * 16.0);
	bme680->fcomp.tfine = var1 + var2;
	temp_comp = (var1 + var2) / 5120.0;
	bme680->fcomp.temp = temp_comp;
}

static void calc_temp_comp_2 (bme680_t *bme680) {

	int32_t var1, var2, var3, temp_comp;

	var1 = ((int32_t)bme680->adc.temp >> 3) - ((int32_t) bme680->cal.par_t1 << 1);
	var2 = (var1 * (int32_t)bme680->cal.par_t2) >> 11;
	var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)bme680->cal.par_t3 << 4)) >> 14;
	bme680->icomp.tfine = var2 + var3;
	temp_comp = (((var2 + var3) * 5) + 128) >> 8;
	bme680->icomp.temp = temp_comp;
}

static void calc_temp_comp (bme680_t *bme680) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		calc_temp_comp_1(bme680);
	} else {
		calc_temp_comp_2(bme680);
	}
}

/********************************************************************/
/********************************************************************/
static void calc_press_comp_1 (bme680_t *bme680) {

	double var1, var2, var3, press_comp;

	var1 = ((double)bme680->fcomp.tfine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double)bme680->cal.par_p6 / 131072.0);
	var2 = var2 + (var1 * (double)bme680->cal.par_p5 * 2.0);
	var2 = (var2 / 4.0) + ((double)bme680->cal.par_p4 * 65536.0);
	var1 = ((((double)bme680->cal.par_p3 * var1 * var1) / 16384.0) +
		((double)bme680->cal.par_p2 * var1)) / 524288.0;
	var1 = (1.0 + (var1 / 32768.0)) * (double)bme680->cal.par_p1;
	press_comp = 1048576.0 - (double)bme680->adc.press;
	press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
	var1 = ((double)bme680->cal.par_p9 * press_comp * press_comp) / 2147483648.0;
	var2 = press_comp * ((double)bme680->cal.par_p8 / 32768.0);
	var3 = (press_comp / 256.0) * (press_comp / 256.0) *
	       (press_comp / 256.0) * (bme680->cal.par_p10 / 131072.0);
	press_comp = press_comp + (var1 + var2 + var3 + ((double)bme680->cal.par_p7 * 128.0)) / 16.0;
	bme680->fcomp.press = press_comp;
}

static void calc_press_comp_2 (bme680_t *bme680 )  {

	int32_t var1, var2, var3, press_comp;

	var1 = ((int32_t)bme680->icomp.tfine >> 1) - 64000;
	var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)bme680->cal.par_p6) >> 2;
	var2 = var2 + ((var1 * (int32_t)bme680->cal.par_p5) << 1);
	var2 = (var2 >> 2) + ((int32_t)bme680->cal.par_p4 << 16);
	var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) *
			((int32_t)bme680->cal.par_p3 << 5)) >> 3) +  (((int32_t)bme680->cal.par_p2 * var1) >> 1);
	var1 = var1 >> 18;
	var1 = ((32768 + var1) * (int32_t)bme680->cal.par_p1) >> 15;
	press_comp = 1048576 - bme680->adc.press; // bosch code pg 19 says "press_raw" here ???
	press_comp = (uint32_t)((press_comp - (var2 >> 12)) * ((uint32_t)3125));
	if (press_comp >= (1 << 30))
		press_comp = ((press_comp / (uint32_t)var1) << 1);
	else
		press_comp = ((press_comp << 1) / (uint32_t)var1);
	var1 = ((int32_t)bme680->cal.par_p9 * (int32_t)(((press_comp >> 3) *
				(press_comp >> 3)) >> 13)) >> 12;
	var2 = ((int32_t)(press_comp >> 2) * (int32_t)bme680->cal.par_p8) >> 13;
	var3 = ((int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) *
			(int32_t)(press_comp >> 8) * (int32_t)bme680->cal.par_p10) >> 17;
	press_comp = (int32_t)(press_comp) + ((var1 + var2 + var3 + ((int32_t)bme680->cal.par_p7 << 7)) >> 4);
	bme680->icomp.press = press_comp;
}

static void calc_press_comp (bme680_t *bme680) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		calc_press_comp_1(bme680);
	} else {
		calc_press_comp_2(bme680);
	}
}

/********************************************************************/
/********************************************************************/
static void calc_hum_comp_1 (bme680_t *bme680) {
	
	double var1, var2, var3, var4, hum_comp, temp_comp;

	temp_comp = bme680->fcomp.temp;

	var1 = bme680->adc.hum - (((double)bme680->cal.par_h1 * 16.0) + (((double)bme680->cal.par_h3 / 2.0) * temp_comp));
	var2 = var1 * (((double)bme680->cal.par_h2 / 262144.0) * (1.0 + (((double)bme680->cal.par_h4 / 16384.0) *
				temp_comp) + (((double)bme680->cal.par_h5 / 1048576.0) * temp_comp * temp_comp)));
	var3 = (double)bme680->cal.par_h6 / 16384.0;
	var4 = (double)bme680->cal.par_h7 / 2097152.0;
	hum_comp = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);

	bme680->fcomp.hum = hum_comp;
}

static void calc_hum_comp_2 (bme680_t *bme680) {

	int32_t  var1, var2, var3, var4, var5, var6, temp_scaled, hum_comp;
	
	temp_scaled = (int32_t)bme680->icomp.temp;
	var1 = (int32_t)bme680->adc.hum - (int32_t)((int32_t)bme680->cal.par_h1 << 4) -
		(((temp_scaled * (int32_t)bme680->cal.par_h3) / ((int32_t)100)) >> 1);
	var2 = ((int32_t)bme680->cal.par_h2 * (((temp_scaled * (int32_t)bme680->cal.par_h4) / ((int32_t)100)) +
		(((temp_scaled * ((temp_scaled * (int32_t)bme680->cal.par_h5) /
		((int32_t)100))) >> 6) / ((int32_t)100)) + ((int32_t)(1 << 14)))) >> 10;
	var3 = var1 * var2;
	var4 = (((int32_t)bme680->cal.par_h6 << 7) +
			((temp_scaled * (int32_t)bme680->cal.par_h7) / ((int32_t)100))) >> 4;
	var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
	var6 = (var4 * var5) >> 1;
	hum_comp = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;

	bme680->icomp.hum = hum_comp;
}

static void calc_hum_comp (bme680_t *bme680) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		calc_hum_comp_1(bme680);
	} else {
		calc_hum_comp_2(bme680);
	}
}

/********************************************************************/

int bme680_calibrate(bme680_t *bme680) {

	uint8_t buffer[3] = {0, 0 ,0};
	int err = 0;

	// start with temp params
	
	err |= read_dev(bme680, 0xE9, buffer, 2);
	bme680->cal.par_t1 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x8A, buffer, 2);
	bme680->cal.par_t2 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x8C, buffer, 1);
	bme680->cal.par_t3 = buffer[0];

	// pressure
	
	err |= read_dev(bme680, 0x8E, buffer, 2);
	bme680->cal.par_p1 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x90, buffer, 2);
	bme680->cal.par_p2 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x92, buffer, 1);
	bme680->cal.par_p3 = buffer[0];

	err |= read_dev(bme680, 0x94, buffer, 2);
	bme680->cal.par_p4 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x96, buffer, 2);
	bme680->cal.par_p5 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x99, buffer, 1);
	bme680->cal.par_p6 = buffer[0];

	err |= read_dev(bme680, 0x98, buffer, 1); // strange order
	bme680->cal.par_p7 = buffer[0];

	err |= read_dev(bme680, 0x9C, buffer, 1);
	bme680->cal.par_p8 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x9E, buffer, 2);
	bme680->cal.par_p9 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0xA0, buffer, 1);
	bme680->cal.par_p10 = buffer[0];

	// humidity
	
	err |= read_dev(bme680, 0xE2, buffer, 2);
	bme680->cal.par_h1 = (buffer[1] << 4) | (buffer[0] & 0xF);

	err |= read_dev(bme680, 0xE1, buffer, 2);
	bme680->cal.par_h2 = (buffer[0] << 4) | ((buffer[1] >> 4) & 0xF);

	err |= read_dev(bme680, 0xE4, buffer, 1);
	bme680->cal.par_h3 = buffer[0];

	err |= read_dev(bme680, 0xE5, buffer, 1);
	bme680->cal.par_h4 = buffer[0];

	err |= read_dev(bme680, 0xE6, buffer, 1);
	bme680->cal.par_h5 = buffer[0];

	err |= read_dev(bme680, 0xE7, buffer, 1);
	bme680->cal.par_h6 = buffer[0];

	err |= read_dev(bme680, 0xE8, buffer, 1);
	bme680->cal.par_h7 = buffer[0];

	// gas
	
	err |= read_dev(bme680, 0xED, buffer, 1);
	bme680->cal.par_g1 = buffer[0];

	err |= read_dev(bme680, 0xEB, buffer, 2);
	bme680->cal.par_g2 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0xEE, buffer, 1);
	bme680->cal.par_g2 = buffer[0];

	//  todo more



	return err;
}


/********************************************************************/

void bme680_print_calibration (bme680_t *bme680) {
	printf("par_t1: %d\n", bme680->cal.par_t1);
	printf("par_t2: %d\n", bme680->cal.par_t2);
	printf("par_t3: %d\n", bme680->cal.par_t3);
	printf("par_p1: %d\n", bme680->cal.par_p1);
	printf("par_p2: %d\n", bme680->cal.par_p2);
	printf("par_p3: %d\n", bme680->cal.par_p3);
	printf("par_p4: %d\n", bme680->cal.par_p4);
	printf("par_p5: %d\n", bme680->cal.par_p5);
	printf("par_p6: %d\n", bme680->cal.par_p6);
	printf("par_p7: %d\n", bme680->cal.par_p7);
	printf("par_p8: %d\n", bme680->cal.par_p8);
	printf("par_p9: %d\n", bme680->cal.par_p9);
	printf("par_p10: %d\n", bme680->cal.par_p10);
	printf("par_h1: %d\n", bme680->cal.par_h1);
	printf("par_h2: %d\n", bme680->cal.par_h2);
	printf("par_h3: %d\n", bme680->cal.par_h3);
	printf("par_h4: %d\n", bme680->cal.par_h4);
	printf("par_h5: %d\n", bme680->cal.par_h5);
	printf("par_h6: %d\n", bme680->cal.par_h6);
	printf("par_h7: %d\n", bme680->cal.par_h7);
}


