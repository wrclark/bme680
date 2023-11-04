#ifndef REGISTERS_H
#define REGISTERS_H

#define REG_SPI_PAGE(v) ((v > 0x7F) ? 0 : 1)

#define REG_STATUS      0x73
#define REG_RESET       0xE0
#define REG_ID          0xD0
#define REG_CONFIG      0x75
#define REG_CTRL_MEAS   0x74
#define REG_CTRL_HUM    0x72

#define REG_CTRL_GAS_1  0x71
#define REG_CTRL_GAS_0  0x70

#define REG_GAS_WAIT_9  0x6D
#define REG_GAS_WAIT_8  0x6C
#define REG_GAS_WAIT_7  0x6B
#define REG_GAS_WAIT_6  0x6A
#define REG_GAS_WAIT_5  0x69
#define REG_GAS_WAIT_4  0x68
#define REG_GAS_WAIT_3  0x67
#define REG_GAS_WAIT_2  0x66
#define REG_GAS_WAIT_1  0x65
#define REG_GAS_WAIT_0  0x64

#define REG_RES_HEAT_9  0x63
#define REG_RES_HEAT_8  0x62
#define REG_RES_HEAT_7  0x61
#define REG_RES_HEAT_6  0x60
#define REG_RES_HEAT_5  0x5F
#define REG_RES_HEAT_4  0x5E
#define REG_RES_HEAT_3  0x5D
#define REG_RES_HEAT_2  0x5C
#define REG_RES_HEAT_1  0x5B
#define REG_RES_HEAT_0  0x5A

#define REG_IDAC_HEAT_9 0x59
#define REG_IDAC_HEAT_8 0x58
#define REG_IDAC_HEAT_7 0x57
#define REG_IDAC_HEAT_6 0x56
#define REG_IDAC_HEAT_5 0x55
#define REG_IDAC_HEAT_4 0x54
#define REG_IDAC_HEAT_3 0x53
#define REG_IDAC_HEAT_2 0x52
#define REG_IDAC_HEAT_1 0x51
#define REG_IDAC_HEAT_0 0x50

#define REG_GAS_R_LSB   0x2B
#define REG_GAS_R_MSB   0x2A

#define REG_HUM_LSB     0x26
#define REG_HUM_MSB     0x25

#define REG_TEMP_XLSB   0x24
#define REG_TEMP_LSB    0x23
#define REG_TEMP_MSB    0x22

#define REG_PRESS_XLSB  0x21
#define REG_PRESS_LSB   0x20
#define REG_PRESS_MSB   0x1F

#define REG_EAS_STATUS  0x1D



#endif
