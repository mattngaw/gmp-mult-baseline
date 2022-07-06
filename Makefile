CC = gcc
CFLAGS = -g -O0 -lgmp -D__FMA__ -frounding-math -Wno-deprecated -march=native -std=c11 -fopenmp
CPPFLAGS := -I./lib/FFT-Integer-Multiplication/ext_libs/IGen/igen_lib -I./lib/FFT-Integer-Multiplication/ext_libs/crlibm -I./lib/FFT-Integer-Multiplication/ext_libs/crlibm/scs_lib -I/ux1/users/mngaw/include
LDFLAGS = -lgmp -L $(FFT_DIR)/ext_libs/crlibm -L $(HOME)/static/lib -L/ux1/users/mngaw/lib -L/usr/lib64
LDLIBS := -lcrlibm -lm -l:libmpfi.a -l:libmpfr.a -l:libgmp.so

SRC_DIR := ./src
BUILD_DIR := ./build
FFT_DIR := ./lib/FFT-Integer-Multiplication

all: $(BUILD_DIR)/main.o $(BUILD_DIR)/fft.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(BUILD_DIR)/main.o $(BUILD_DIR)/fft.o -o main $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/fft.o: $(FFT_DIR)/libs/fft.c $(FFT_DIR)/libs/fft.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(FFT_DIR)/libs/fft.c -o $(BUILD_DIR)/fft.o $(LDFLAGS) $(LDLIBS)

clean:
	rm -f ./main; rm -f ./build/*
