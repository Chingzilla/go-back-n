//Authors:
//   Chris and Jiwan
//
#include "sendto_.h"

struct GBNAck{
    int seq_num;              // Sequence number
    int recv_win_size;         // Size of the receiver's window
}

//**********************************************
// Function definition:
//**********************************************

// Function to send ack struct
// Return value -> number of bytes sent to the remote addr defined by struct sockaddr sento
int send_ack(int socket_handler,struct sockaddr sendto, socklen_t sendto_len, GBNAck *ack);


// Function to get ack 
// Return value -> GBNAck struct
GBNACk get_ack (int socket_handler.struct sockaddr* from, socketlen_t from_len);



