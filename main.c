/* usleep() */
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>
#include "bme680.h"
#include "i2c.h"

#define DEVICE "/dev/i2c-1"
#define ADDRESS 0x77

int main(){
	
	int fd = i2c_init(DEVICE, ADDRESS);
	bme680_t bme680;

	printf("%X\n", i2c_read_reg2(fd, 0xD0));
	if (bme680_calibrate(fd, &bme680.cal) != 0) {
	       fprintf(stderr, "error bme680 calibration\n");
	} else {
 		print_calibration(&bme680.cal);
	}		



	//1. write osrs fields to diff regs
	uint8_t ctrl_meas, ctrl_hum;

	uint8_t osrs_t, osrs_p, osrs_h, ctrl_mode;
	osrs_t = osrs_p = osrs_h = 0b110;
	ctrl_mode = 0;

	ctrl_meas = (osrs_t << 5) | (osrs_p << 2) | ctrl_mode;
	ctrl_hum = osrs_h;

	i2c_write_reg(fd, 0x74, ctrl_meas);
	i2c_write_reg(fd, 0x72, ctrl_hum);

	// set filter mode 
	
	uint8_t filter_reg = 0b101 << 2;
	i2c_write_reg(fd, 0x75, filter_reg);
	
	// todo gas stuff
	
	// resend ctrl_meas but with ctrl_mode set to 0b01 in it, then it should work
	ctrl_meas |= 1;
	i2c_write_reg(fd,  0x74, ctrl_meas);

	// poll reg `meas_status_0 bit 5. It will be 0 when all scheduled conversions are done.
	uint8_t qq;
	while ((qq = i2c_read_reg2(fd, 0x1D)) >> 4) {
		usleep(2000); // 2 ms
	}

	// now read the temp/press/hum adc and convert in order.
	uint32_t temp_adc, press_adc, hum_adc;


	uint8_t buffer[3];

	i2c_read_reg(fd, 0x22, 3, buffer);
	temp_adc = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);

	i2c_read_reg(fd, 0x1F, 3, buffer);
	press_adc = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);

	i2c_read_reg(fd, 0x25, 2, buffer);
	hum_adc = (buffer[0] << 8) | buffer[1];

	// adc readings are only 20-bit when the IIR filter is enabled, otherwise
	// the size depends on the oversample settings (osrs_X).
	
	double temperature = calc_temp_comp_1 ( temp_adc ,  &bme680 );
	printf("temperature: %g oC (floating-point)\n", temperature);
	
	int temperature2 = calc_temp_comp_2 (temp_adc,  &bme680 );
	printf("temperature: %d oC (integer x100)\n", temperature2);


	double pressure = calc_press_comp_1 ( press_adc  , &bme680 );
	printf("pressure: %g Pa or %g bar (floating-point)\n", pressure, pressure/1e5);

	int pressure2 = calc_press_comp_2 (press_adc  ,  &bme680 );
	printf("pressure: %d Pa (integer)\n", pressure2);

	double humidity = calc_hum_comp_1 ( hum_adc, &bme680 );
	printf("humidity: %g (floating-point)\n", humidity);

	int humidity2 = calc_hum_comp_2 ( hum_adc,  &bme680 );
	printf("humidity: %d (integer x1000)\n", humidity2);

	close(fd);
	return 0;
}
