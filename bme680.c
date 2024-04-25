#include <stdio.h>

#include "bme680.h"
#include "registers.h"


static void calc_temp_comp(bme680_t *bme680);
static void calc_press_comp(bme680_t *bme680);
static void calc_hum_comp(bme680_t *bme680);
static void calc_gas_res(bme680_t *bme680);

static void check_spi_page(bme680_t *bme680, uint8_t reg);
static int set_spi_page(bme680_t *bme680, uint8_t no);


/********************************************************************/
static int write_dev(bme680_t *bme680, uint8_t reg, uint8_t value) {
	if (BME680_IS_SPI(bme680->mode)) {
		check_spi_page(bme680, reg);
		reg &= 0x7F;
	}

	return bme680->dev.write(reg, value);
}

/********************************************************************/
static int read_dev(bme680_t *bme680, uint8_t reg, uint8_t *dst, uint32_t size) {
	if (BME680_IS_SPI(bme680->mode)) {
		check_spi_page(bme680, reg);
		reg |= 0x80; 
	}

	return bme680->dev.read(reg, dst, size);
}

/********************************************************************/
/* change spi page if necessary */
static void check_spi_page(bme680_t *bme680, uint8_t reg) {
	uint8_t required_page = REG_SPI_PAGE(reg);

	if (required_page != bme680->spi_page) {
		set_spi_page(bme680, required_page);
		bme680->spi_page = required_page;
	}
}

/********************************************************************/
static int set_spi_page(bme680_t *bme680, uint8_t page_no) {
	uint8_t status_reg = page_no << 4;
	return bme680->dev.write(REG_STATUS, status_reg);
}

/********************************************************************/
static int read_id(bme680_t *bme680, uint8_t *id) {
	uint8_t id_reg;

	/* force spi page 0. special case */
	if (BME680_IS_SPI(bme680->mode)) {
		set_spi_page(bme680, 0);
		bme680->spi_page = 0;
		id_reg = 0x50 | 0x80;
	} else {
		id_reg = REG_ID;
	}

	return bme680->dev.read(id_reg, id, 1);
}

/********************************************************************/
/* write local setpoint index to device, must not be running */
int bme680_write_setpoint_index(bme680_t *bme680) {
	/* setpoint (0 thru 9) bits 0,1,2,3 and run_gas bit 4 */
	uint8_t ctrl_gas_1 = (bme680->setpoint & 0x0F) | (1 << 4);
	return write_dev(bme680, REG_CTRL_GAS_1, ctrl_gas_1);
}

/********************************************************************/
/* read the currently selected heater setpoint index on the device */
int bme680_read_setpoint_index(bme680_t *bme680, uint8_t *index) {
	uint8_t meas_status;
	int err = 0;

	err |= read_dev(bme680, REG_MEAS_STATUS, &meas_status, 1);
	*index = (meas_status) & 0x0F;

	return err;
}

/********************************************************************/
int bme680_init(bme680_t *bme680, uint8_t mode) {
	uint8_t id;
	int i;

	bme680->mode = mode;
	bme680->spi_page = 0; 
	bme680->gas_valid = 0;
	bme680->heat_stab = 0;
	bme680->setpoint = 0;

	if (bme680->dev.init() != 0) {
		return 1;
	}

	if (read_id(bme680, &id) != 0) {
		return 1;
	}

	if (id != 0x61) {
		return 1;
	}

	/* zero gas sensor arrays */
	for(i=0; i<10; i++) {
		bme680->cfg.idac_heat[i] = 0;
		bme680->cfg.res_heat[i] = 0;
		bme680->cfg.gas_wait[i] = 0;
	}

	return 0;
}

/********************************************************************/
int bme680_deinit(bme680_t *bme680) {
	if (bme680->dev.deinit) {
		bme680->dev.deinit();
	}

	return 0;
}

/********************************************************************/
int bme680_reset(bme680_t *bme680) {
	uint8_t magic = 0xB6;
	uint8_t reg;
	int ret;

	/* force page 0. special case */
	if (BME680_IS_SPI(bme680->mode)) {
		set_spi_page(bme680, 0);
		bme680->spi_page = 0;
		reg = 0x60 | 0x80;
	} else {
		reg = REG_RESET;
	}

	ret = bme680->dev.write(reg, magic);
	bme680->dev.sleep(2000); /* sleep for 2 ms */

	return ret;
}

/********************************************************************/
/* configure device */
int bme680_configure(bme680_t *bme680) {
	uint8_t meas, hum, filter, ctrl_gas1, ctrl_gas0, err;
	int i;
	meas = hum = filter = err = 0;

	/* ctrl_meas. the last 0 is ticked on to enable forced mode,
	 * but the config has to be written first. strange behaviour.
	 */
	meas   = bme680->cfg.osrs_t << 5 | bme680->cfg.osrs_p << 2; 
	hum    = bme680->cfg.osrs_h;
	filter = bme680->cfg.filter << 2;

	/* backup of ctrl meas reg because you cannot retrieve it from the device later */
	bme680->cfg.meas = meas;

	err |= write_dev(bme680, REG_CTRL_MEAS, meas);
	err |= write_dev(bme680, REG_CTRL_HUM,  hum);
	err |= write_dev(bme680, REG_CONFIG,  filter);

	if (!BME680_GAS_ENABLED(bme680->mode)) {
		goto SKIP_GAS;
	}

	/* write out all 10 setpoints */
	/* those not explicitly set are defaulted to 0 (which has no effect) */
	for(i=0; i<10; i++) {
		err |= write_dev(bme680, 0x6D - i, bme680->cfg.gas_wait[9 - i]);	
		err |= write_dev(bme680, 0x63 - i, bme680->cfg.res_heat[9 - i]);	
		err |= write_dev(bme680, 0x59 - i, bme680->cfg.idac_heat[9 - i]);	
	}

	ctrl_gas1 = bme680->setpoint | (1 << 4);
	ctrl_gas0 = 0; /* := (1 << 3) to turn off current going to heater */

	err |= write_dev(bme680, REG_CTRL_GAS_1, ctrl_gas1);
	err |= write_dev(bme680, REG_CTRL_GAS_0, ctrl_gas0);

SKIP_GAS:
	return err;
}

/********************************************************************/
/* To start forced mode, you just have to set the lsb=1 of REG_CTRL_MEAS */
int bme680_start(bme680_t *bme680) {
	int err = 0;
	uint8_t meas;
	meas = bme680->cfg.meas | 1;
	err |= write_dev(bme680, REG_CTRL_MEAS, meas);
	return err;
}

/********************************************************************/
/* blocks until all scheduled conversions on the device are done. */
int bme680_poll(bme680_t *bme680) {
	uint8_t meas_status = 0;
	uint8_t gas_measuring = 0;
	uint8_t any_measuring = 0;
	int err = 0;


	do {
		bme680->dev.sleep(5000); /* 5 ms */
		err |= read_dev(bme680, REG_MEAS_STATUS, &meas_status, 1);
		gas_measuring = (meas_status >> 6) & 1;
		any_measuring = (meas_status >> 5) & 1;

	} while ((gas_measuring || any_measuring) && !err);

	return err;
}

/********************************************************************/
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
	 * IIR filter off -> 16 + (osrs_x - 1) bits.
	 * */
	if (bme680->cfg.filter == BME680_IIR_COEFF_0) {
		bme680->adc.temp  >>= (bme680->cfg.osrs_t - 1);
		bme680->adc.press >>= (bme680->cfg.osrs_p - 1);
	}

	/* read gas adc values and check error bits */
	if (BME680_GAS_ENABLED(bme680->mode)) {

		err |= read_dev(bme680, 0x2A, buffer, 2);
		
		/* read gas-related adc values */
		bme680->adc.gas = (buffer[0] << 2) | (buffer[1] >> 6);
		bme680->adc.gas_range = buffer[1] & 0xF;

		/* check gas validity status (if one actually took place ??? ) */
		bme680->gas_valid = (buffer[1] >> 5) & 1;

		/* check heater stability. if it managed to get to temp within given time + preload current */
		bme680->heat_stab = (buffer[1] >> 4) & 1;
	}

	/* read/convert in order ..*/
	calc_temp_comp(bme680);
	calc_press_comp(bme680);
	calc_hum_comp(bme680);
	
	if (BME680_GAS_ENABLED(bme680->mode)) {
		calc_gas_res(bme680);
	}

	return err;
}

/***********************************************************************/
/*  These arrays are used to compute a sensor heating value `res_heat' */
/*  for a specified heating target, specified in degree C.             */
/***********************************************************************/
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

/* long int maybe */
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

/********************************************************************/
static void calc_temp_comp_2 (bme680_t *bme680) {
	int32_t var1, var2, var3, temp_comp;

	var1 = ((int32_t)bme680->adc.temp >> 3) - ((int32_t) bme680->cal.par_t1 << 1);
	var2 = (var1 * (int32_t)bme680->cal.par_t2) >> 11;
	var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)bme680->cal.par_t3 << 4)) >> 14;
	bme680->icomp.tfine = var2 + var3;
	temp_comp = (((var2 + var3) * 5) + 128) >> 8;
	bme680->icomp.temp = temp_comp;
}

/********************************************************************/
static void calc_temp_comp (bme680_t *bme680) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		calc_temp_comp_1(bme680);
	} else {
		calc_temp_comp_2(bme680);
	}
}

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

/********************************************************************/
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

/********************************************************************/
static void calc_press_comp (bme680_t *bme680) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		calc_press_comp_1(bme680);
	} else {
		calc_press_comp_2(bme680);
	}
}

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

/********************************************************************/
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

/********************************************************************/
static void calc_hum_comp (bme680_t *bme680) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		calc_hum_comp_1(bme680);
	} else {
		calc_hum_comp_2(bme680);
	}
}

/********************************************************************/
// TODO: read one big contiguous block
int bme680_calibrate(bme680_t *bme680) {
	uint8_t buffer[3] = {0, 0 ,0};
	int err = 0;

	/* temperature */
	err |= read_dev(bme680, 0xE9, buffer, 2);
	bme680->cal.par_t1 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x8A, buffer, 2);
	bme680->cal.par_t2 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x8C, buffer, 1);
	bme680->cal.par_t3 = buffer[0];


	/* pressure */
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

	err |= read_dev(bme680, 0x98, buffer, 1);
	bme680->cal.par_p7 = buffer[0];

	err |= read_dev(bme680, 0x9C, buffer, 1);
	bme680->cal.par_p8 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0x9E, buffer, 2);
	bme680->cal.par_p9 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0xA0, buffer, 1);
	bme680->cal.par_p10 = buffer[0];


	/* humidity */
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


	/* gas */	
	err |= read_dev(bme680, 0xED, buffer, 1);
	bme680->cal.par_g1 = buffer[0];

	err |= read_dev(bme680, 0xEB, buffer, 2);
	bme680->cal.par_g2 = (buffer[1] << 8) | buffer[0];

	err |= read_dev(bme680, 0xEE, buffer, 1);
	bme680->cal.par_g3 = buffer[0];

	err |= read_dev(bme680, 0x04, buffer, 1);
	bme680->cal.range_switching_error = buffer[0];

	err |= read_dev(bme680, 0x02, buffer, 1);
	bme680->cal.res_heat_range = (buffer[0] >> 4) & 3;

	err |= read_dev(bme680, 0x00, buffer, 1);
	bme680->cal.res_heat_val = buffer[0];

	return err;
}

/********************************************************************/
static void calc_gas_res_1(bme680_t *bme680) {
	double var1, gas_res;
	var1 = (1340.0 + 5.0 * bme680->cal.range_switching_error) * const_array1[bme680->adc.gas_range];
	gas_res = var1 * const_array2[bme680->adc.gas_range] / (bme680->adc.gas - 512.0 + var1);
	bme680->fcomp.gas_res = gas_res;
}

/********************************************************************/
static void calc_gas_res_2(bme680_t *bme680) {
	int64_t var1, var2;
	int32_t gas_res;

	var1 = (int64_t)(((1340 + (5 * (int64_t)bme680->cal.range_switching_error)) *
			((int64_t)const_array1_int[bme680->adc.gas_range])) >> 16);
	var2 = (int64_t)(bme680->adc.gas << 15) - (int64_t)(1 << 24) + var1;
	gas_res = (int32_t)((((int64_t)(const_array2_int[bme680->adc.gas_range] * 
			(int64_t)var1) >> 9) + (var2 >> 1)) / var2);
	bme680->icomp.gas_res = gas_res;
}

/********************************************************************/
static void calc_gas_res(bme680_t *bme680) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		calc_gas_res_1(bme680);
	} else {
		calc_gas_res_2(bme680);
	}
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
	printf("par_g1: %d\n", bme680->cal.par_g1);
	printf("par_g2: %d\n", bme680->cal.par_g2);
	printf("par_g3: %d\n", bme680->cal.par_g3);
	printf("range_switching_error: %d\n", bme680->cal.range_switching_error);
	printf("res_heat_range: %d\n", bme680->cal.res_heat_range);
	printf("res_heat_val: %d\n", bme680->cal.res_heat_val);
}

/********************************************************************/
static uint8_t calc_target_1(bme680_t *bme680, double target, double ambient) {
	double var1, var2, var3, var4, var5;
	uint8_t res_heat;

	var1 = ((double)bme680->cal.par_g1 / 16.0) + 49.0;
	var2 = (((double)bme680->cal.par_g2 / 32768.0) * 0.0005) + 0.00235;
	var3 = (double)bme680->cal.par_g3 / 1024.0;
	var4 = var1 * (1.0 + (var2 * (double)target)); /* */
	var5 = var4 + (var3 * (double)ambient);
	res_heat = (uint8_t)(3.4 * ((var5 * (4.0 / (4.0 + (double)bme680->cal.res_heat_range)) *
			(1.0 / (1.0 + ((double)bme680->cal.res_heat_val * 0.002)))) - 25));
	return res_heat;
}

/********************************************************************/
static uint8_t calc_target_2(bme680_t *bme680, double target, double ambient) {
	int32_t var1, var2, var3, var4, var5, res_heat_x100;
	uint8_t res_heat;

	var1 = (((int32_t)ambient * bme680->cal.par_g3) / 10) << 8;
	var2 = (bme680->cal.par_g1 + 784) * (((((bme680->cal.par_g2 + 154009) * target * 5) /
			100) + 3276800) / 10);
	var3 = var1 + (var2 >> 1);
	var4 = (var3 / (bme680->cal.res_heat_range + 4));
	var5 = (131 * bme680->cal.res_heat_val) + 65536;
	res_heat_x100 = (int32_t)(((var4 / var5) - 250) * 34);
	res_heat = (uint8_t)((res_heat_x100 + 50) / 100);
	return res_heat;
}

/********************************************************************/
uint8_t bme680_calc_target(bme680_t *bme680, double target, double ambient) {
	if (BME680_IS_FLOAT(bme680->mode)) {
		return calc_target_1(bme680, target, ambient);
	} else {
		return calc_target_2(bme680, target, ambient);
	}
}

