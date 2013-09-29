#include "gbnack.h"

int send_ack(GBNAck * self, int socket_handler, struct sockaddr_in sendto){
    int bytes_sent;
    char c_seq_num[5];
    char c_recv_win_size[5];
    int MAXACKSIZE =5+4; 
    char buffer[MAXACKSIZE];
    
    // Convert seq_num and recv_win_size to string
    snprintf(c_seq_num,5,  "%d",self->seq_num);
    snprintf(c_recv_win_size,5,  "%d",self->recv_win_size);

    // Copy seq_num and recv_win_size to buffer
    memcpy(buffer, c_seq_num, 5);
    memcpy(buffer+4, c_recv_win_size, 5);

    // Send the buffer
    bytes_sent= sendto_(socket_handler,buffer, MAXACKSIZE ,0, (struct sockaddr*) &sendto, sizeof(sendto));

    return bytes_sent;
}


int get_ack(GBNAck* self, int socket_handler, struct sockaddr_in from) {
    int MAXACKSIZE = 5+4;
    char buffer[MAXACKSIZE];
    char c_seq_num[5];
    char c_recv_win_size[5];
    int bytes_rcvd;
    unsigned int from_len;

    bytes_rcvd = recvfrom(socket_handler,buffer, MAXACKSIZE, 0, (struct sockaddr*) &from, &from_len);
    if (bytes_rcvd > 0){
        // Copy the seq_num and recv_win_size
        memcpy(c_seq_num, buffer, 4);
        memcpy(c_recv_win_size, buffer+4, 4);

        self->seq_num = atoi(c_seq_num);
        self->recv_win_size = atoi(c_recv_win_size);
    }
    return bytes_rcvd;   
}
