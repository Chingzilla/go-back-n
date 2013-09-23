# tests/Makefile

CFLAGS = -Wall -ggdb -std=c99 -pthread -lcheck 

all:

test: test_rbw
	./test_rbw

test_rbw: test_rbw.c ringbufferwindow.o
	gcc ${CFLAGS} -o test_rbw test_rbw.c ringbufferwindow.o

ringbufferwindow.o: ../src/ringbufferwindow.c
	gcc ${CFLAGS} -c ../src/ringbufferwindow.c
