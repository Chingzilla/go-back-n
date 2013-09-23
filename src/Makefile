#!/usr/bin/make -f

CC = gcc
CFLAGS = -ggdb -std=c99 -Wall

libs = ringbufferwindow.o

all: ${libs}

client: client.c ${libs}
	false

server: server.c ${libs}
	false

