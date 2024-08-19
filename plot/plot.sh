#!/usr/bin/env sh

sed -n '/1$/p' ../data/spi.txt > spi.txt
sed -n '/1$/p' ../data/i2c.txt > i2c.txt
gnuplot gnuplot.gpt