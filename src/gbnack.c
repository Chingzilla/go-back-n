#include "gbnack.h"

int send_ack(GBNAck self, int socket_handler, struct sockaddr_in sendto, int addr_len){
    int bytes_sent;
    uint16_t n_seq_num, n_rev_win_size;
    char buffer[MAXACKSIZE];
    
    // Convert seq_num and rev_win_size to string
    n_seq_num = htons(self->seq_num);
    n_rev_win_size = htons(self->rev_win_size);

    //printf("Send ack %4d:%d\n", self->seq_num, self->rev_win_size);

    // Copy seq_num and rev_win_size to buffer
    memcpy(buffer, (char*)&n_seq_num, 2);
    memcpy(buffer+2, (char*)&n_rev_win_size, 2);

    // Send the buffer
    bytes_sent= sendto_(socket_handler, buffer, MAXACKSIZE ,0, (struct sockaddr*) &sendto, addr_len);

    if(bytes_sent == MAXACKSIZE){
        return 0;
    }
    return 1;
}


int get_ack(GBNAck self, int socket_handler, struct sockaddr_in from) {
    int bytes_rcvd;
    int *sender_size;

    char buffer[MAXACKSIZE];

    bytes_rcvd = recvfrom(socket_handler,buffer, MAXACKSIZE, 0, (struct sockaddr*) &from, sender_size);
    //if (bytes_rcvd != MAXACKSIZE){
    //    return 1;
    //}

    // Copy the seq_num and rev_win_size
    self->seq_num = ntohs(*((uint16_t*)buffer));
    self->rev_win_size = ntohs(*((uint16_t*)(buffer+2)));

    printf("Ack %d:%d\n", self->seq_num, self->rev_win_size);
    
    return 1;
}
