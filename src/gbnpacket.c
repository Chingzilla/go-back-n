#include "gbnpacket.h"

double send_packet(int socket_handler,struct sockaddr sendto, socklen_t sendto_len, GBNPacket *packet){
  struct timeval now;
  int bytes_sent;
  double time_in_mill;

  gettimeofday(&now, NULL);
  time_in_mill = (tv.tv_sec)*1000 + (tv.tv_usec)/1000;
  packet->send_time = time_in_mill;

	bytes_sent= sendto_(socket_handler, &packet, sizeof(packet), 0, (struct sockaddr*) &sendto, sendto_len);

	return bytes_sent;
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
