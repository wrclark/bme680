/*
 * This file is part of bme680.
 *
 * Copyright (C) 2025 William Clark
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <stdio.h>
#include "bme680_util.h"

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