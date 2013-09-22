#include "gbnack.h"

int send_ack(int socket_handler,struct sockaddr sendto, socklen_t sendto_len, GBNAck *ack){
	int bytes_sent;

	bytes_sent= sendto_(socket_handler, &ack, sizeof(ack) ,0, (struct sockaddr*) &sendto, sendto_len);

	return nbytes_sent;

}


GBNAck get_ack (int socket_handler, struct sockaddr* from, socketlen_t from_len) {
	struct GBNAck rcvd_ack;
   	int bytes_rcvd;

    bytes_rcvd = recvfrom(socket_handler, &rcvd_ack, sizeof (rcvd_ack), 0, (struct sockaddr*) &from, from_len);
    if (bytes_rcvd > 0){
    	    // Set the received flag on
    	    rcvd_packet.recvd = 1;	
    }
    return rcvd_ack;   
}
