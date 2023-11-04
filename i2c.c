#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include "i2c.h"

int i2c_init(const char *dev, uint8_t addr) {
	int fd = -1;

	fd = open(dev, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "could not open device: %s\n", dev);
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		fprintf(stderr, "failed to acquire bus/talk to slave\n");
		close(fd);
		return -1;
	}

	return fd;
}


int i2c_read_reg(int fd, uint8_t reg, uint8_t *dst, uint32_t size) {

	uint8_t cmd[2] = {reg, 0x00};
	write(fd, cmd, 2);
	
	if (read(fd, dst, size) != (ssize_t)size) {
		fprintf(stderr, "error read()\n");
		return I2C_ERR;
	
	}
	return I2C_OK;
}


int i2c_write_reg(int fd, uint8_t reg, uint8_t value) {
	
	uint8_t cmd[2] = {reg, value};
	
	if (write(fd, cmd, 2) != 2) {
		fprintf(stderr, "error write()\n");
		return I2C_ERR;
	}

	return I2C_OK;
}

