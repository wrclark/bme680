#include "bme680.h"

const double const_array1[16] = {
	1,
	1,
	1,
	1,
	1,
	0.99,
	1, 
	0.992,
       	1,
       	1,
       	0.998,
       	0.995, 
	1,
       	0.99,
       	1, 
	1
};

const double const_array2[16] = {
	8000000,
       	4000000,
       	2000000,
       	1000000,
       	499500.4995,
       	248262.1648,
       	125000,
	63004.03226, 
	31281.28128, 
	15625, 
	7812.5, 
	3906.25, 
	1953.125,
	976.5625,
       	488.28125,
       	244.140625
};

const int const_array1_int[16] = {
	2147483647, 
	2147483647, 
	2147483647, 
	2147483647, 
	2147483647, 
	2126008810,
	2147483647,
	2130303777,
	2147483647,
	2147483647,
	2143188679,
	2136746228,
	2147483647,
	2126008810,
	2147483647,
	2147483647
};

const int const_array2_int[16] = {
	4096000000, // too big ?
	2048000000,
	1024000000,
	512000000,
	255744255,
	127110228,
	64000000,
	32258064,
	16016016,
	8000000,
	4000000,
	2000000,
	1000000,
	500000,
	250000,
	125000
};


static double calc_temp_comp_1 ( uint32_t temp_adc , bme680_t *bme680) {

	double var1, var2, temp_comp;
	bme680_calibration *cal;

	cal = &bme680->cal;


	var1 = (((double)temp_adc / 16384.0) - ((double)cal->par_t1 / 1024.0)) * 
		(double)cal->par_t2;
	var2 = ((((double)temp_adc / 131072.0) - ((double)cal->par_t1 / 8192.0)) *
		(((double)temp_adc / 131072.0) - ((double)cal->par_t1 / 8192.0))) *
			((double)cal->par_t3 * 16.0);
	bme680->tfine_double = var1 + var2;
	temp_comp = (var1 + var2) / 5120.0;
	bme680->temp_comp_double = temp_comp;
	return temp_comp;
}

static int calc_temp_comp_2 (uint32_t temp_adc, bme680_t *bme680) {

	int32_t var1, var2, var3, temp_comp;
	bme680_calibration *cal;

	cal = &bme680->cal;

	var1 = ((int32_t)temp_adc >> 3) - ((int32_t) cal->par_t1 << 1);
	var2 = (var1 * (int32_t)cal->par_t2) >> 11;
	var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)cal->par_t3 << 4)) >> 14;
	bme680->tfine_int = var2 + var3;
	temp_comp = (( (var2 + var3) * 5) + 128) >> 8;
	bme680->temp_comp_int = temp_comp;
	return temp_comp;
}

static double calc_press_comp_1 ( uint32_t press_adc  ,  bme680_t *bme680 ) {

	double var1, var2, var3, press_comp;
	bme680_calibration *cal;

	cal = &bme680->cal;
	
	var1 = (bme680->tfine_double / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double)cal->par_p6 / 131072.0);
	var2 = var2 + (var1 * (double)cal->par_p5 * 2.0);
	var2 = (var2 / 4.0) + ((double)cal->par_p4 * 65536.0);
	var1 = ((((double)cal->par_p3 * var1 * var1) / 16384.0) +
			((double)cal->par_p2 * var1)) / 524288.0;
	var1 = (1.0 + (var1 / 32768.0)) * (double)cal->par_p1;
	press_comp = 1048576.0 - (double)press_adc;
	press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
	var1 = ((double)cal->par_p9 * press_comp * press_comp) / 2147483648.0;
	var2 = press_comp * ((double)cal->par_p8 / 32768.0);
	var3 = (press_comp / 256.0) * (press_comp / 256.0) * (press_comp / 256.0) * (cal->par_p10 / 131072.0);
	press_comp = press_comp + (var1 + var2 + var3 + ((double)cal->par_p7 * 128.0)) / 16.0;
	bme680->press_comp_double = press_comp;
	return press_comp;
}


static int calc_press_comp_2  ( uint32_t press_adc ,  bme680_t *bme680 )  {

	uint32_t var1, var2, var3, press_comp;
	bme680_calibration *cal;

	cal = &bme680->cal;

	var1 = ((int32_t)bme680->tfine_int >> 1) - 64000;
	var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)cal->par_p6) >> 2;
	var2 = var2 + ((var1 * (int32_t)cal->par_p5) << 1);
	var2 = (var2 >> 2) + ((int32_t)cal->par_p4 << 16);
	var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) *
			((int32_t)cal->par_p3 << 5)) >> 3) + (((int32_t)cal->par_p2 * var1) >> 1);
	var1 = var1 >> 18;
	var1 = ((32768 + var1) * (int32_t)cal->par_p1) >> 15;
	press_comp = 1048576 - press_adc; // says "press_raw" in the bosch example
	press_comp = (uint32_t)((press_comp - (var2 >> 12)) * ((uint32_t)3125));
	if (press_comp >= (1 << 30))
		press_comp = ((press_comp / (uint32_t)var1) << 1);
	else
		press_comp = ((press_comp << 1) / (uint32_t)var1);
	var1 = ((int32_t)cal->par_p9 * (int32_t)(((press_comp >> 3) *
			(press_comp >> 3)) >> 13)) >> 12;
	var2 = ((int32_t)(press_comp >> 2) * (int32_t)cal->par_p8) >> 13;
	var3 = ((int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) *
			(int32_t)(press_comp >> 8) * (int32_t)cal->par_p10) >> 17;
	press_comp = (int32_t)(press_comp) + ((var1 + var2 + var3 + ((int32_t)cal->par_p7 << 7)) >> 4);
	
	bme680->press_comp_int = press_comp;
	
	return press_comp;

}

static double calc_hum_comp_1  ( uint32_t hum_adc  ,   bme680_t *bme680 ) {
	
	double var1, var2, var3, var4, hum_comp;
	bme680_calibration *cal;
	cal = &bme680->cal;

	var1 = hum_adc - (((double)cal->par_h1 * 16.0) + (((double)cal->par_h3 / 2.0) * bme680->temp_comp_double));
	var2 = var1 * (((double)cal->par_h2 / 262144.0) * (1.0 + (((double)cal->par_h4 / 16384.0) *
				bme680->temp_comp_double) + (((double)cal->par_h5 / 1048576.0) *
					bme680->temp_comp_double * bme680->temp_comp_double)));
	var3 = (double)cal->par_h6 / 16384.0;
	var4 = (double)cal->par_h7 / 2097152.0;
	hum_comp = var2 + ((var3 + (var4 * bme680->temp_comp_double)) * var2 * var2);
	bme680->hum_comp_double = hum_comp;
	return hum_comp;
}


static int calc_hum_comp_2  (  uint32_t hum_adc  ,  bme680_t *bme680 ) {

	uint32_t  var1, var2, var3, var4, var5, var6, temp_scaled, hum_comp;
	bme680_calibration *cal;
	
	cal = &bme680->cal;

	temp_scaled = (int32_t)(bme680->temp_comp_int);
	var1 = (int32_t)hum_adc - (int32_t)((int32_t)cal->par_h1 << 4) -
		(((temp_scaled * (int32_t)cal->par_h3) / ((int32_t)100)) >> 1);
	var2 = ((int32_t)cal->par_h2 * (((temp_scaled *
		(int32_t)cal->par_h4) / ((int32_t)100)) +
		(((temp_scaled * ((temp_scaled * (int32_t)cal->par_h5) /
		((int32_t)100))) >> 6) / ((int32_t)100)) + ((int32_t)(1 << 14)))) >> 10;
	var3 = var1 * var2;
	var4 = (((int32_t)cal->par_h6 << 7) +
		((temp_scaled * (int32_t)cal->par_h7) / ((int32_t)100))) >> 4;
	var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
	var6 = (var4 * var5) >> 1;
	hum_comp = (((var3 + var6) >> 10) * ((int32_t)1000)) >> 12;
	bme680->hum_comp_int = hum_comp;
	return hum_comp;
}



