#include "bme680.h"
#include "registers.h"



int bme680_init(bme680_t *bme680) {

	uint8_t car;

	if (bme680->dev.init() != 0) {
		return 1;
	}

	if (bme680->dev.read(REG_ID, &car, 1) != 0) {
		return 1;
	}

	if (car != 0x61) {
		return 1;
	}

	return 0;
}


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


int bme680_calibrate(bme680_t *bme680) {

	uint8_t buffer[3] = {0, 0 ,0};
	int err = 0;

	// start with temp params
	
	err |= bme680->dev.read(0xE9, buffer, 2);
	bme680->cal.par_t1 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0x8A, buffer, 2);
	bme680->cal.par_t2 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0x8C, buffer, 1);
	bme680->cal.par_t3 = buffer[0];

	// pressure
	
	err |= bme680->dev.read(0x8E, buffer, 2);
	bme680->cal.par_p1 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0x90, buffer, 2);
	bme680->cal.par_p2 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0x92, buffer, 1);
	bme680->cal.par_p3 = buffer[0];

	err |= bme680->dev.read(0x94, buffer, 2);
	bme680->cal.par_p4 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0x96, buffer, 2);
	bme680->cal.par_p5 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0x99, buffer, 1);
	bme680->cal.par_p6 = buffer[0];

	err |= bme680->dev.read(0x98, buffer, 1); // strange order
	bme680->cal.par_p7 = buffer[0];

	err |= bme680->dev.read(0x9C, buffer, 1);
	bme680->cal.par_p8 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0x9E, buffer, 2);
	bme680->cal.par_p9 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0xA0, buffer, 1);
	bme680->cal.par_p10 = buffer[0];

	// humidity
	
	err |= bme680->dev.read(0xE2, buffer, 2);
	bme680->cal.par_h1 = (buffer[1] << 4) | (buffer[0] & 0xF);

	err |= bme680->dev.read(0xE1, buffer, 2);
	bme680->cal.par_h2 = (buffer[0] << 4) | ((buffer[1] >> 4) & 0xF);

	err |= bme680->dev.read(0xE4, buffer, 1);
	bme680->cal.par_h3 = buffer[0];

	err |= bme680->dev.read(0xE5, buffer, 1);
	bme680->cal.par_h4 = buffer[0];

	err |= bme680->dev.read(0xE6, buffer, 1);
	bme680->cal.par_h5 = buffer[0];

	err |= bme680->dev.read(0xE7, buffer, 1);
	bme680->cal.par_h6 = buffer[0];

	err |= bme680->dev.read(0xE8, buffer, 1);
	bme680->cal.par_h7 = buffer[0];

	// gas
	
	err |= bme680->dev.read(0xED, buffer, 1);
	bme680->cal.par_g1 = buffer[0];

	err |= bme680->dev.read(0xEB, buffer, 2);
	bme680->cal.par_g2 = (buffer[1] << 8) | buffer[0];

	err |= bme680->dev.read(0xEE, buffer, 1);
	bme680->cal.par_g2 = buffer[0];

	//  todo more



	return err;
}

/*
void print_calibration  ( bme680_calibration *cal ) {
	printf("par_t1: %d\n", cal->par_t1);
	printf("par_t2: %d\n", cal->par_t2);
	printf("par_t3: %d\n", cal->par_t3);
	printf("par_p1: %d\n", cal->par_p1);
	printf("par_p2: %d\n", cal->par_p2);
	printf("par_p3: %d\n", cal->par_p3);
	printf("par_p4: %d\n", cal->par_p4);
	printf("par_p5: %d\n", cal->par_p5);
	printf("par_p6: %d\n", cal->par_p6);
	printf("par_p7: %d\n", cal->par_p7);
	printf("par_p8: %d\n", cal->par_p8);
	printf("par_p9: %d\n", cal->par_p9);
	printf("par_p10: %d\n", cal->par_p10);
	printf("par_h1: %d\n", cal->par_h1);
	printf("par_h2: %d\n", cal->par_h2);
	printf("par_h3: %d\n", cal->par_h3);
	printf("par_h4: %d\n", cal->par_h4);
	printf("par_h5: %d\n", cal->par_h5);
	printf("par_h6: %d\n", cal->par_h6);
	printf("par_h7: %d\n", cal->par_h7);
} */


