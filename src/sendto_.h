#ifndef h_sendto
#define h_sendto

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


static int p_threshold;
static int c_threshold;
static int count = 1;

void init_net_lib(double probability, unsigned int seed);

int sendto_(int sock, void* message, int length, int flags, struct sockaddr* sa, int dest_len);

#endif
