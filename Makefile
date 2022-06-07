CC = gcc
CFLAGS = -g -std=c99 -lgmp
LDFLAGS = -lgmp

SRC_DIR := ./src
BUILD_DIR := ./build

all: gmp_baseline

gmp_baseline: $(BUILD_DIR)/gmp_baseline.o
	$(CC) $(BUILD_DIR)/gmp_baseline.o $(LDFLAGS) -o gmp_baseline

$(BUILD_DIR)/gmp_baseline.o: $(SRC_DIR)/gmp_baseline.c
	$(CC) -c $(SRC_DIR)/gmp_baseline.c $(CFLAGS) -o $(BUILD_DIR)/gmp_baseline.o

clean:
	rm -f ../bin/*

