CC=gcc
CFLAGS=-static -fno-pie -no-pie -Wall -Wextra -pedantic -O0
BINARY=dijkstra
LINKER=../link.ld
OBJS=
TARGET=dijkstra.c

default:
	@echo "Compiling ${BINARY}..."
	${CC} ${CFLAGS} ${LINKER} ${TARGET} ${OBJS} -o ${BINARY} -lm
	cp ${BINARY} ../a.out

