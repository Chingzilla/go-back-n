// Authors: 
//   Chris and Jiwan
//
#ifndef h_gbnpacket
#define h_gbnpacket

#include <sys/time.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "sendto_.h"

#define MAXDATASIZE 1024

typedef struct{   
    char data[MAXDATASIZE];   // data
    int size;                 // size of data
    int seq_num;              // sequence number   
    
    int recvd;                         // packet recvd flag (returns 1->true, 0->false)
    double send_time;            // timer value
} GBNPacketObj,*GBNPacket;

//**********************************************
// Function definition:
//**********************************************

// Function to send packet struct
// Return value -> number of bytes sent to the remote addr defined by struct sockaddr sento
int send_packet(GBNPacket self, int socket_handler, struct sockaddr_in sendto);

// Function to get packet struct
// Return value -> number of bytes received
int get_packet (GBNPacket self, int socket_handler, struct sockaddr_in from);

// Function that resets the recvd and send_time flag
// Return value -> void
void clear(GBNPacket self);      

// Function to get currenct time in milliseconds
// Return value-> time in milliseconds
double get_time_in_millisecs();

#endif
