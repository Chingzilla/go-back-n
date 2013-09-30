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
    int size_header = 4+1; // +1 for the null terminator
    int total_buf_size = size_header + MAXDATASIZE; // the null terminator is already accounted for in size_header
    char buffer[total_buf_size]; 

    // Convert seq num from int to char
    snprintf(buffer,size_header,  "%d",self->seq_num);
    // Copy data from packet to buffer
    memcpy(buffer+4, self->data, MAXDATASIZE);

    // Send the packet to the server
    bytes_sent= sendto_(socket_handler,buffer, MAXDATASIZE, 0, (struct sockaddr *) &sendto, sizeof(sendto));
    // If  packet sent
    if(bytes_sent >0)
    { 
           // Set the send_time of the packet
           self->send_time = get_time_in_millisecs();
    }
    return bytes_sent;
}


int get_packet (GBNPacket self,int socket_handler, struct sockaddr_in from){
    int bytes_rcvd;
    int MAXBUFSIZE = 1024 + 5; // data + header
    char buffer[MAXBUFSIZE];
    char header[5];
    unsigned int from_len;

    bytes_rcvd = recvfrom(socket_handler, buffer, MAXBUFSIZE , 0, (struct sockaddr*) &from, &from_len);
    if (bytes_rcvd > 0){
        // copy the sequence number to the packet = first 4 bytes
        memcpy(header, buffer , 4);
        self->seq_num = atoi(header);

         // copy the received data to the packet
        memcpy(self->data, buffer+4, MAXDATASIZE);

        //set the received flag on
        self->recvd =1;
    }
    return bytes_rcvd;
}

int gbnp_copy(GBNPacket self, GBNPacket other){
    if( memcpy(self->data, other->data, MAXDATASIZE) == NULL){
        fprintf(stderr, "Error copying data to packet\n");
        return 1;
    }
    return 0;
}

void clear(GBNPacket self){
	self->recvd = 0;
	self->send_time = 0;
}
