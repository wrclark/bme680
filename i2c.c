#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#define MY_DEVICE "/dev/i2c-1"
#define MY_DEVICE_ADDRESS 0x77

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


int i2c_read_reg(int fd, uint8_t reg, uint8_t length, uint8_t *buf) {

	uint8_t cmd[2] = {reg, 0x00};
	write(fd, cmd, 2);
	if (read(fd, buf, length) != length) {
		fprintf(stderr, "error read()\n");
		return 1;
	}
	return 0;
}

uint8_t i2c_read_reg2(int fd, uint8_t reg) {
	uint8_t buf;
	i2c_read_reg(fd, reg, 1, &buf);
	return buf;
}

int i2c_write_reg(int fd, uint8_t reg, uint8_t value) {
	
	uint8_t cmd[2] = {reg, value};
	if (write(fd, cmd, 2) != 2) {
		fprintf(stderr, "error write()\n");
		return 1;
	}
	return 0;
}

int main2(void) {


	int res;
	uint8_t cmd_buf[2] = {0x00, 0x00};
	int fd = i2c_init(MY_DEVICE, MY_DEVICE_ADDRESS);


	if (i2c_read_reg(fd, 0xD0, 1, (uint8_t *)&res) != 0) {
		fprintf(stderr, "ddd\n");
	}

	printf("%X\n", res);

	printf("%X\n", i2c_read_reg2(fd, 0xD0));

	close(fd);
	return 0;
}
