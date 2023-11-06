#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "spi.h"

#define SPI_DEVICE "/dev/spidev0.0"


/*
 * Pinout config for this example
 *
 * MOSI - GPIO 10 (physical pin 19)
 * MISO - GPIO 9  (physical pin 21)
 * SCLK - GPIO 11 (physical pin 23)
 * CE0  - GPIO 8  (physical pin 24)
 *
 */

#define SPI_MODE 0
#define SPI_BITS 8
#define SPI_SPEED 500 * 1000 /* 500 KHz */
#define SPI_DELAY 0

static int fd;

//static void print_access(const char *name, uint8_t reg, uint8_t *dst, uint32_t size);

int spi_init(void) {

	uint8_t mode = SPI_MODE;
	uint8_t bits = SPI_BITS;
	uint32_t speed = SPI_SPEED;

	if ((fd = open(SPI_DEVICE, O_RDWR)) < 0) {
		fprintf(stderr, "spi open(%s)\n", SPI_DEVICE);
		return SPI_ERR;
	}


	if (ioctl(fd, SPI_IOC_RD_MODE, &mode) == -1) {
		fprintf(stderr, "SPI_IOC_RD_MODE\n");
		return SPI_ERR;
	}

	if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) {
		fprintf(stderr, "SPI_IOC_WR_MODE\n");
		return SPI_ERR;
	}

	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
		fprintf(stderr, "SPI_IOC_WR_BITS_PER_WORD\n");
		return SPI_ERR;
	}

	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1) {
		fprintf(stderr, "SPI_IOC_RD_BITS_PER_WORD\n");
		return SPI_ERR;
	}

	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
		fprintf(stderr, "SPI_IOC_WR_MAX_SPEED_HZ\n");
		return SPI_ERR;
	}

	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) {
		fprintf(stderr, "SPI_IOC_RD_MAX_SPEED_HZ\n");
		return SPI_ERR;
	}


//	puts("spi_init");
	return SPI_OK;
}

/* bme680 auto-increments for spi reads */
int spi_read(uint8_t reg, uint8_t *dst, uint32_t size) {
	
	struct spi_ioc_transfer tr[2] = {0};
	reg |= 0x80;

	/* write first msg for the register addr */
	tr[0].tx_buf = (unsigned long) &reg;
	tr[0].rx_buf = (unsigned long) 0; 
	tr[0].len = 1;

	/* write second msg for rx data */
	tr[1].tx_buf = (unsigned long) 0;
	tr[1].rx_buf = (unsigned long) dst;
	tr[1].len = size;

	if (ioctl(fd, SPI_IOC_MESSAGE(2), tr) < 0) {
		fprintf(stderr, "spi_read()\n");
		return SPI_ERR;
	}

//	print_access("spi_read", reg, dst, size);
	return SPI_OK;
}

/* bme680 does NOT auto-increment for spi writes, so one at a time */
int spi_write(uint8_t reg, uint8_t value) {

	struct spi_ioc_transfer tr[2] = {0};
	reg &= 0x7F;

	/* write first msg for the register addr */
	tr[0].tx_buf = (unsigned long) &reg;
	tr[0].rx_buf = (unsigned long) 0;
	tr[0].len = (uint32_t) 1;

	/* write second msg for the tx data */
	tr[1].tx_buf = (unsigned long) &value;
	tr[1].rx_buf = (unsigned long) 0;
	tr[1].len = (uint32_t) 1;

	if (ioctl(fd, SPI_IOC_MESSAGE(2), tr) < 0) {
		fprintf(stderr, "spi_write()\n");
		return SPI_ERR;
	}

//	print_access("spi_write", reg, &value, 1);
	return SPI_OK;
}

int spi_deinit(void) {
//	puts("spi_deinit");
	if (fd) {
		close(fd);
	}

	return SPI_OK;
}

/*
static void print_access(const char *name, uint8_t reg, uint8_t *dst, uint32_t size) {
	printf("%s: %.2X (%d) [", name, reg, size);
	for(uint32_t i=0; i<size; i++) {
		printf("%.2X", dst[i]);
		if (i < (size - 1)) {
			printf(", ");
		}
	}
	printf("]\n");
}
*/

