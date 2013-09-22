#include "gbnpacket.h"


int send_packet(int socket_handler,struct sockaddr sendto, socklen_t sendto_len, GBNPacket *packet){
	int bytes_sent;

  
	bytes_sent= sendto_(socket_handler, &packet, sizeof(packet), 0, (struct sockaddr*) &sendto, sendto_len);

	return 0;
}


GBNPacket get_packet (int socket_handler, struct sockaddr from, socketlen_t from_len){
   	struct GBNPacket rcvd_packet;
   	int bytes_rcvd;

    bytes_rcvd = recvfrom(socket_handler, &rcvd_packet, sizeof (rcvd_packet), 0, (struct sockaddr*) &from, from_len);
    if (bytes_rcvd > 0){
    	    // Set the received flag on
    	    rcvd_packet.recvd = 1;	
    }
    return rcvd_packet;   
}

void clear(GBNPacket *packet){
	packet->recvd = 0;
	packet->send_time = 0;
}
