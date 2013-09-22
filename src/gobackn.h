/*** src/gobackn.h
 * Authors: Chris Ching and Jiwan Rosen
 ***/

#ifndef h_gobackn
#define h_gobackn

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

//#include "ringbufferwindow.h"

struct __ringbufferwindow; // Solution for circular dependency

#define DATASIZE 1024

//TODO: Add ACK and packet stuff
typedef struct {
    struct __ringbufferwindow *buff;
    //TODO
} *GBNSender;

typedef struct {
    struct __ringbufferwindow *buff;
    //TODO
} *GBNReceiver;

typedef struct {
    //TODO
    char data[DATASIZE];
    int seq_num;
} *GBNPacket;

typedef struct {
    int seq_num;
    int rev_win_size;
} *GBNAck;

#endif // h_gobackn
