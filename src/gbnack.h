//Authors:
//   Chris and Jiwan
//

#ifndef h_gbnack
#define h_gbnack

#include "sendto_.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>

typedef struct GBNAck{
    int seq_num;              // Sequence number
    int recv_win_size;         // Size of the receiver's window
}GBNAck;

//**********************************************
// Function definition:
//**********************************************

// Function to send ack struct
// Return value -> number of bytes sent to the remote addr defined by struct sockaddr sento
int send_ack(GBNAck * self, int socket_handler, struct sockaddr_in sendto);

// Function to get ack 
// Return value -> number of bytes received
int get_ack(GBNAck* self, int socket_handler, struct sockaddr_in from);

#endif

