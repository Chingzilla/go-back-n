#!/usr/bin/make -f

CC = gcc
CFLAGS = -ggdb -std=c99

libs = ringbufferwindow.o gbnpacket.o gbnack.o sendto_.o

test_args = 5000 0.10 45
test_args_server = ${test_args} tmp/file_out tmp/server_log
test_args_client = 127.0.0.1 ${test_args} tmp/file_in tmp/client_log

all: GBNclient GBNserver

test: test_server

clean:
	rm -f ${libs}
	rm -f GBNclient
	rm -f GBNserver
	rm -f tmp/{file_out,server_log,client_log,server_stdout,client_stdout}

GBNclient: GBNclient.c ${libs}
	${CC} ${CFLAGS} -o GBNclient GBNclient.c ${libs}

GBNserver: GBNserver.c ${libs}
	${CC} ${CFLAGS} -o GBNserver GBNserver.c ${libs}

test_server: GBNserver
	#./GBNserver ${test_args_server} | tee tmp/server_stdout
	./GBNserver ${test_args_server}
	md5sum tmp/file_in tmp/file_out

test_client: GBNclient
	#./GBNclient ${test_args_client} | tee tmp/client_stdout
	./GBNclient ${test_args_client}

.PHONY: all clean test test_server test_client
