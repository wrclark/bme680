# bme680

Example implementation, just lacking proven SPI connectivity for now.


```
i2c_init
i2c_read: D0 (1) [61]
i2c_write: E0 [B6]
i2c_read: E9 (2) [5B, 66]
i2c_read: 8A (2) [97, 67]
i2c_read: 8C (1) [03]
i2c_read: 8E (2) [C0, 88]
i2c_read: 90 (2) [D4, D7]
i2c_read: 92 (1) [58]
i2c_read: 94 (2) [DC, 25]
i2c_read: 96 (2) [36, FF]
i2c_read: 99 (1) [1E]
i2c_read: 98 (1) [18]
i2c_read: 9C (1) [FC]
i2c_read: 9E (2) [73, F2]
i2c_read: A0 (1) [1E]
i2c_read: E2 (2) [FA, 31]
i2c_read: E1 (2) [3E, FA]
i2c_read: E4 (1) [00]
i2c_read: E5 (1) [2D]
i2c_read: E6 (1) [14]
i2c_read: E7 (1) [78]
i2c_read: E8 (1) [9C]
i2c_read: ED (1) [D0]
i2c_read: EB (2) [85, E9]
i2c_read: EE (1) [12]
i2c_read: 04 (1) [13]
i2c_read: 02 (1) [16]
i2c_read: 00 (1) [2E]
par_t1: 26203
par_t2: 26519
par_t3: 3
par_p1: 35008
par_p2: -10284
par_p3: 88
par_p4: 9692
par_p5: -202
par_p6: 30
par_p7: 24
par_p8: -4
par_p9: -3469
par_p10: 30
par_h1: 794
par_h2: 1007
par_h3: 0
par_h4: 45
par_h5: 20
par_h6: 120
par_h7: -100
par_g1: 208
par_g2: 59781
par_g3: 18
range_switching_error: 19
res_heat_range: 1
res_heat_val: 46
i2c_write: 74 [B4]
i2c_write: 72 [04]
i2c_write: 75 [1C]
i2c_write: 6D [BC]
i2c_write: 63 [E7]
i2c_write: 59 [28]
i2c_write: 6C [BC]
i2c_write: 62 [E7]
i2c_write: 58 [28]
i2c_write: 6B [BC]
i2c_write: 61 [E7]
i2c_write: 57 [28]
i2c_write: 6A [BC]
i2c_write: 60 [E7]
i2c_write: 56 [28]
i2c_write: 69 [BC]
i2c_write: 5F [E7]
i2c_write: 55 [28]
i2c_write: 68 [BC]
i2c_write: 5E [E7]
i2c_write: 54 [28]
i2c_write: 67 [BC]
i2c_write: 5D [E7]
i2c_write: 53 [28]
i2c_write: 66 [BC]
i2c_write: 5C [E7]
i2c_write: 52 [28]
i2c_write: 65 [BC]
i2c_write: 5B [E7]
i2c_write: 51 [28]
i2c_write: 64 [BC]
i2c_write: 5A [E7]
i2c_write: 50 [28]
i2c_write: 71 [10]
i2c_write: 70 [00]
i2c_write: 74 [B5]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
...many identical lines omitted...
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [20]
i2c_read: 1D (1) [80]
i2c_read: 22 (3) [75, 3D, D0]
i2c_read: 1F (3) [55, 22, 30]
i2c_read: 25 (2) [63, 2E]
i2c_read: 2A (2) [00, 28]
i2c_read: 2B (1) [28]
float mode
tfine: 98700.755011
temp: 19.277491 degC
press: 96363.204250 Pa
humidity: 68.749455 % RH
gas resistance: 48633.411307 Ohm
== for heater target=320.0 and ambient temp=25.0 (degC)
i2c_deinit
```
Low air pressure due to storm not bad sensor
