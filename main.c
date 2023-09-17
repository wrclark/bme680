#include <stdio.h>
#include "bme680.h"
#include "i2c.h"

#define DEVICE "/dev/i2c-1"
#define ADDRESS 0x77

int main(){
	
	int fd = i2c_init(DEVICE, ADDRESS);

	printf("%X\n", i2c_read_reg2(fd, 0xD0));

	close(fd);
	return 0;
}
