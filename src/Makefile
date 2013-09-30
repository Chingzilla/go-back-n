#!/usr/bin/make -f

CC = gcc
CFLAGS = -ggdb -std=c99

libs = ringbufferwindow.o gbnpacket.o gbnack.o sendto_.o

all: GBNclient GBNserver

test: test_server

clean:
	rm -f ${libs}
	rm -f GBNclient
	rm -f GBNserver

GBNclient: GBNclient.c ${libs}
	${CC} ${CFLAGS} -o GBNclient GBNclient.c ${libs}

GBNserver: GBNserver.c ${libs}
	${CC} ${CFLAGS} -o GBNserver GBNserver.c ${libs}

test_server: GBNserver
	./GBNserver 5000 0.0 42 tmp/file_out tmp/server_log

.PHONY: all clean test test_server
