//Authors:
//   Chris and Jiwan
//

#ifndef h_gbnack
#define h_gbnack

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include "sendto_.h"

#define MAXACKSIZE 4

typedef struct {
    int seq_num;              // Sequence number
    int rev_win_size;         // Size of the receiver's window
}GBNAckObj, *GBNAck;

//**********************************************
// Function definition:
//**********************************************

// Function to send ack struct
// Return value -> number of bytes sent to the remote addr defined by struct sockaddr sento
int send_ack(GBNAck self, int socket_handler, struct sockaddr_in sendto, int addr_len);

// Function to get ack 
// Return value -> number of bytes received
int get_ack(GBNAck self, int socket_handler, struct sockaddr_in from);

#endif

