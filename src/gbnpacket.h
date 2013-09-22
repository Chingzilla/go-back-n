// Authors: 
//   Chris and Jiwan
//

#include <sys/time.h>
#include "sendto_.h"

#define MAXDATASIZE 1024

struct GBNPacket{   
    char data[MAXDATASIZE];   // data
    int size;                 // size of data
    int seq_num;              // sequence number   
    
    int recvd;                // packet recvd flag (returns 1->true, 0->false)
    double send_time;            // timer value
}

//**********************************************
// Function definition:
//**********************************************

// Function to send packet struct
// Return value -> number of bytes sent to the remote addr defined by struct sockaddr sento
int send_packet(int socket_handler,struct sockaddr *sendto, socklen_t sendto_len, GBNPacket *packet);


// Function to get packet struct
// Return value -> GBNPacket struct
GBNPacket get_packet (int socket_handler, struct sockaddr from, socketlen_t from_len);


// Function that resets the recvd and send_time flag
// Return value -> void
void clear(GBNPacket *packet);       

    
