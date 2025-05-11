CC=gcc
CFLAGS = -g -O0 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wfloat-equal \
         -Wconversion -Wcast-align -Wpointer-arith -Wstrict-overflow=5 \
         -Wwrite-strings -Wcast-qual -Wswitch-default -Wswitch-enum \
         -Wunreachable-code -Wstrict-prototypes -Wmissing-prototypes \
         -Wmissing-declarations -Wredundant-decls -Wold-style-definition \
         -Winline -Wlogical-op -Wjump-misses-init -Wdouble-promotion \
         -Wformat-overflow=2 -Wformat-signedness -Wmissing-include-dirs \
         -Wnull-dereference -Wstack-usage=1024 -Wpacked -Woverlength-strings \
         -Wvla -Walloc-zero -Wduplicated-cond -Wduplicated-branches -Wrestrict \
         -fanalyzer -I. -std=c89

CFILES = $(wildcard *.c)
OBJECTS = $(CFILES:.c=.o)
BIN = bme680

all: spi i2c log

log: $(OBJECTS)
	@echo ">>> $(BIN)_log_spi"
	@$(CC) $(CFLAGS) -DUSE_SPI -std=gnu99 $^ cmd/log.c -o "$(BIN)_log_spi"
	@echo ">>> $(BIN)_log_i2c"
	@$(CC) $(CFLAGS) -DUSE_I2C -std=gnu99 $^ cmd/log.c -o "$(BIN)_log_i2c"

spi: $(OBJECTS)
	@echo ">>> $(BIN)_spi"
	@$(CC) $(CFLAGS) -DUSE_SPI $^ cmd/main.c -o "$(BIN)_spi"

i2c: $(OBJECTS)
	@echo ">>> $(BIN)_i2c"
	@$(CC) $(CFLAGS) -DUSE_I2C $^ cmd/main.c -o "$(BIN)_i2c"

%.o:%.c
	@echo "cc $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJECTS) $(BIN) $(BIN)_spi $(BIN)_i2c $(BIN)_log_spi $(BIN)_log_i2c 