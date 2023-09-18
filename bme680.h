#pragma once

#include <stdint.h>

typedef struct {
	uint16_t par_t1;
	uint16_t par_t2;
	uint16_t par_t3; // 8 bit word
	uint16_t par_p1;
	uint16_t par_p2;
	uint16_t par_p3; // 8 bit word
	uint16_t par_p4;
	uint16_t par_p5;
	uint16_t par_p6; // 8 bit word
	uint16_t par_p7; // 8 bit word
	uint16_t par_p8; 
	uint16_t par_p9;
	uint16_t par_p10; //8 bit word
	uint16_t par_h1; 
	uint16_t par_h2;
	uint16_t par_h3;
	uint16_t par_h4;
	uint16_t par_h5;
	uint16_t par_h6;
	uint16_t par_h7;
	uint16_t par_g1;
	uint16_t par_g2;
	uint16_t par_g3;
	uint16_t range_switching_error;
} bme680_calibration;

typedef struct {

	int tfine_int;
	double tfine_double;

	double temp_comp_double;
	int temp_comp_int;

	double press_comp_double;
	int press_comp_int;

	double hum_comp_double;
	int hum_comp_int;

	bme680_calibration cal;

} bme680_t;



void print_calibration(bme680_calibration *);
int bme680_calibrate(int, bme680_calibration *);
int calc_hum_comp_2(uint32_t, bme680_t *);
double calc_hum_comp_1(uint32_t, bme680_t *);
int calc_press_comp_2(uint32_t, bme680_t *);
double calc_press_comp_1(uint32_t, bme680_t *);
int calc_temp_comp_2(uint32_t, bme680_t *);
double calc_temp_comp_1(uint32_t, bme680_t*);


