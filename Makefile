all:
	gcc -O2 -std=c99 -Wall -Wextra main.c bme680.c i2c.c -o bme680
