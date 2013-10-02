#include "gbnpacket.h"

double get_time_in_millisecs(){
    struct timeval now;
    double time_in_mill;

     // Get the current time in milliseconds
    gettimeofday(&now, NULL);
    time_in_mill = (now.tv_sec)*1000 + (now.tv_usec)/1000;
    return time_in_mill;
}

int send_packet(GBNPacket self, int socket_handler, struct sockaddr_in sendto){
    int bytes_sent;
    char buffer[PACKETSIZE]; 

    // Convert to network byte order
    uint16_t n_seq_num = htons(self->seq_num);
    memcpy(buffer, (char *)&n_seq_num, 2);
    // Copy data from packet to buffer
    memcpy(buffer+PHEADERSIZE, self->data, MAXDATASIZE);

    // Send the packet to the server
    bytes_sent= sendto_(socket_handler,buffer, PACKETSIZE, 0, (struct sockaddr *) &sendto, sizeof(sendto));
    // If  packet sent
    if(bytes_sent >0)
    { 
           // Set the send_time of the packet
           self->send_time = get_time_in_millisecs();
    }
    return bytes_sent;
}


int get_packet (GBNPacket self,int socket_handler, struct sockaddr_in *from, int *addr_len){
    int bytes_rcvd;
    char buffer[PACKETSIZE];

    bytes_rcvd = recvfrom(socket_handler, buffer, PACKETSIZE , 0, (struct sockaddr*)from, addr_len);
    printf("Received %s\n", inet_ntoa(from->sin_addr));
    if (bytes_rcvd > 0){
        uint16_t n_seq_num = *((uint16_t*)buffer);
        self->seq_num = ntohs(n_seq_num);
        
         // copy the received data to the packet
        memcpy(self->data, buffer+PHEADERSIZE, MAXDATASIZE);

        self->size = bytes_rcvd - PHEADERSIZE;

        //set the received flag on
        self->recvd =1;
    }
    return bytes_rcvd;
}

int gbnp_copy(GBNPacket self, GBNPacket other){
    
    if( memcpy(self, other, sizeof(GBNPacketObj)) == NULL){
        fprintf(stderr, "Error copying data to packet\n");
        return 1;
    }
    return 0;
}

void clear(GBNPacket self){
	self->recvd = 0;
	self->send_time = 0;
}
