CFLAGS  = -mwindows
CFLAGS += -mthreads
CFLAGS += -D_MT
CFLAGS += -Wall
CFLAGS += -Wextra
# CFLAGS += -pedantic
CFLAGS += -O2
CFLAGS += -m64

ALL_FILES = $(wildcard *.c)

all:
	gcc $(ALL_FILES) -o mandelbrot-set-obfuscated.exe $(CFLAGS)
