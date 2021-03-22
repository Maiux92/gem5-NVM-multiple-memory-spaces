CC=gcc
CFLAGS=-static -fno-pie -no-pie -Wall -Wextra -pedantic -O0 -pthread
BINARY=susan
LINKER=../link.ld
OBJS=
TARGET=susan.c

default:
	@echo "Compiling ${BINARY}..."
	${CC} ${CFLAGS} ${LINKER} ${TARGET} ${OBJS} -o ${BINARY} -lm
	cp ${BINARY} ../a.out

