/*** src/ringbufferwindow.c
 * Authors: Chris Ching and Jiwan Rosen
 ***/

#include "ringbufferwindow.h"

int get_seq_num(int n){
    return (n + BUFFSIZE) % BUFFSIZE; // Adding buffsize so negative values of n work
}

int get_seq_diff(int n1, int n2){
    if (n1 > n2){
        n2 = n2 + BUFFSIZE;
    }
    return n2 - n1;
}

int seq_add(int n1, int n2){
    return (n1 + n2 + BUFFSIZE) % BUFFSIZE;
}

/*** RingBufferWindow class ***/

/* 'private' functions */

// clears packages from window's head to n packages
int rbw_clear_to_n(RingBufferWindow self, int n){
    for (int i = 0; i < n; i++){
        // TODO
        //gbnp_clear(get_packet_n(self, i))
    }
    return 0;
}

/* 'public' functions */

int rbw_init(RingBufferWindow *pself, int win_size){
    *pself = malloc(sizeof(RingBufferWindowObj));

    RingBufferWindow self = *pself;
    self->win_head = 0;
    rbw_set_win_size(self, win_size);
    return 0;
}

GBNPacket rbw_get_packet_n(RingBufferWindow self, int n){
    int seq_n = get_seq_num( self->win_head + n );
    return self->buffer[seq_n];
}

GBNPacket rbw_get_next_packet(RingBufferWindow self, GBNPacket packet){
    int seq_next = get_seq_num( packet->seq_num + 1 );
    return self->buffer[seq_next];
}

int rbw_get_ack_n(RingBufferWindow self, int n, GBNAck ack){
    GBNPacket packet_n = rbw_get_packet_n(self, n);
    ack->seq_num = packet_n->seq_num;
    ack->rev_win_size = self->win_size;

    if ( n < 0 || n >= self->win_size ){
        fprintf(stderr, "Requested ack is outsize window\n");
        return 1;
    }
    return 0;
}

int rbw_get_ack_of_packet(RingBufferWindow self, GBNAck ack, GBNPacket packet){
    ack->seq_num = packet->seq_num;
    ack->rev_win_size = self->win_size;
    return 0;
}

int rbw_inc_head(RingBufferWindow self, int n){
    self->win_head = get_seq_num(self->win_head + n);

    // Check if n is outsize of the window size, return warning if it is
    if( n >= self->win_size || n < 0 ){
        fprintf(stderr, "Window head moved outside previous window\n");
        return 1;
    } else {
        return 0;
    }
}

int rbw_inc_head_to_packet(RingBufferWindow self, GBNPacket packet){
    int seq_diff = get_seq_diff(self->win_head, packet->seq_num);
    return rbw_inc_head(self, seq_diff);
}

int rbw_set_win_size(RingBufferWindow self, int size){
    if ( size < 0 || size > ( BUFFSIZE / 2 - 1 )){
        fprintf(stderr, "Error: tried to set window size to %d\n", size);
        return 1;
    }
    self->win_size = size;
    return 0;
}

int rbw_put_packet(RingBufferWindow self, GBNPacket packet){
    GBNPacket my_packet = self->buffer[packet->seq_num];
    //TODO
    //gbnp_copy(packet, my_packet);
    return 0;
}

// Returns next packet in window, returns NULL if at the end of window
GBNPacket rbw_get_next_in_window(RingBufferWindow self, GBNPacket curr_packet){
    int next_n = get_seq_diff(self->win_head, curr_packet->seq_num);
    if( next_n < 0 || next_n >= self->win_size){
        return NULL;
    }
    return rbw_get_packet_n(self, next_n);
}

/*** end of RingBufferWindow class ***/
