all:
	gcc -O2 -std=gnu99 -Wall -Wextra main.c bme680.c i2c.c -o bme680
