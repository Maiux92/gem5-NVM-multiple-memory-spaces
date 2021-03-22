CC=gcc
CFLAGS=-static -fno-pie -no-pie -Wall -Wextra -pedantic -O0
BINARY=fft
LINKER=../link.ld
OBJS=fftmisc.c fourierf.c
TARGET=main.c

default:
	@echo "Compiling ${BINARY}..."
	${CC} ${CFLAGS} ${LINKER} ${TARGET} ${OBJS} -o ${BINARY} -lm
	cp ${BINARY} ../a.out

