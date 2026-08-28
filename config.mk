
#CC ?= gcc
CC = m68k-apple-macos-gcc

CFLAGS = -Wall -O3 -std=c99 -DNDEBUG
CFLAGS += -Wno-unused-function -m68040 -mhard-float

# OpenMP (optional)
# CFLAGS += -fopenmp

LDFLAGS =
