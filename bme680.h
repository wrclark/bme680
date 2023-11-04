#ifndef BME680_H
#define BME680

#include <stdint.h>

#define BME680_IS_SPI(m)      ((m & 1) == 1)
#define BME680_IS_FLOAT(m)   (((m >> 1) & 1) == 0)

/* connection modes */
#define BME680_SPI   1
#define BME680_I2C   0

/* calculation modes; int or float calc */
#define BME680_MODE_INT   2
#define BME680_MODE_FLOAT 0

#define BME680_MAX_XFER_SIZE 128

/* config values */
#define BME680_OVERSAMPLE_1X  0b001
#define BME680_OVERSAMPLE_2X  0b010
#define BME680_OVERSAMPLE_4X  0b011
#define BME680_OVERSAMPLE_8X  0b100
#define BME680_OVERSAMPLE_16X 0b101

/* IIR filter */
#define BME680_IIR_COEFF_0   0b000
#define BME680_IIR_COEFF_1   0b001
#define BME680_IIR_COEFF_3   0b010
#define BME680_IIR_COEFF_7   0b011
#define BME680_IIR_COEFF_15  0b100
#define BME680_IIR_COEFF_31  0b101
#define BME680_IIR_COEFF_63  0b110
#define BME680_IIR_COEFF_127 0b111


/* user supplied spi/i2c functions */
struct bme680_dev {
	int (*init)   (void);
	int (*read)   (uint8_t reg, uint8_t *dst, uint32_t size);
	int (*write)  (uint8_t reg, uint8_t *src, uint32_t size);
	int (*xfer)   (uint8_t reg, uint8_t *src, uint8_t *dst, uint32_t size);
	int (*deinit) (void);
};

struct bme680_cal {
	/* temp calibration */
	uint16_t par_t1;
	int16_t  par_t2;
	int8_t   par_t3;

	/* press calibration */
	uint16_t par_p1;
	int16_t  par_p2;
	int8_t   par_p3; 
	int16_t  par_p4;
	int16_t  par_p5;
	int8_t   par_p6; 
	int8_t   par_p7; 
	int16_t  par_p8; 
	int16_t  par_p9;
	uint8_t  par_p10;

	/* humidity calibration */
	uint16_t par_h1; 
	uint16_t par_h2;
	int8_t   par_h3;
	int8_t   par_h4;
	int8_t   par_h5;
	uint8_t  par_h6;
	int8_t   par_h7;

	/* gas maybe */
	uint16_t par_g1;
	uint16_t par_g2;
	uint16_t par_g3;
	uint16_t range_switching_error;
};


struct bme680_config {
	uint8_t osrs_t;
	uint8_t osrs_p;
	uint8_t osrs_h;
	uint8_t filter;
	uint8_t heater_setpoint[10];
	uint8_t heater_exposure_ms[10];
	uint8_t heater_exposure_scalar[10];
};

struct bme680_comp_float {
	double tfine;
	double temp;
	double press;
	double hum;
};

struct bme680_comp_int {
	int32_t tfine;
	int32_t temp;
	int32_t press;
	int32_t hum;
};

struct bme680_adc {
	uint32_t temp;
	uint32_t press;
	uint32_t hum;
	uint16_t gas;
};

struct bme680 {
	struct bme680_comp_float fcomp;
	struct bme680_comp_int   icomp;
	struct bme680_cal cal;
	struct bme680_dev dev;
	struct bme680_config cfg;
	struct bme680_adc adc;
	uint8_t mode;
	uint8_t spi_page; 
};

typedef struct bme680 bme680_t;


int bme680_init(bme680_t *bme680, uint8_t mode);
int bme680_deinit(bme680_t *bme680);
int bme680_reset(bme680_t *bme680);
int bme680_calibrate(bme680_t *bme680);
int bme680_start(bme680_t *bme680);
int bme680_poll(bme680_t *bme680);
int bme680_read(bme680_t *bme680);


void bme680_print_calibration(bme680_t *bme680);

#endif
