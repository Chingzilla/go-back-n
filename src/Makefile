#!/usr/bin/make -f

CC = gcc
CFLAGS = -ggdb -std=c99 -Wall

libs = ringbufferwindow.o gbnpacket.o gbnack.o sendto_.o

all: GBNclient GBNserver

clean:
	rm -f ${libs}
	rm -f GBNclient
	rm -f GBNserver

GBNclient: GBNclient.c ${libs}
	${CC} ${CFLAGS} -o GBNclient GBNclient.c ${libs}

GBNserver: GBNserver.c ${libs}
	${CC} ${CFLAGS} -o GBNserver GBNserver.c ${libs}

.PHONY: all clean
