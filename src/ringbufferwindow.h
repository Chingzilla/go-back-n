/*** src/ringbufferwindow.h
 * Authors: Chris Ching and Jiwan Rosen
 ***/

#ifndef ringbufferwindow_h
#define ringbufferwindow_h

#include "gobackn.h"

//TODO: Add ring buffer window
#define BUFFSIZE 100

/** class RingBufferWindow  **/
typedef __ringbufferwindow struct {
    GBNPacket buffer[BUFFSIZE];
    int win_size;
    int win_head;
} *RingBufferWindow;

// Initialize RingBufferWindow
int rbw_init(RingBufferWindow *pself, int win_size, int win_head);

// Get packet in buffer matching (seq number) 
// Returns packet that is at offset 'n' from window head
GBNPacket get_packet_n(int n);

// Returns next packet based on the given packet's sequence number
GBNPacket get_next_packet(GBNPacket packet);

// Fills passed ack with proper information of the packet offset 'n' from window head
int get_ack_n(int n, GBNAck *ack);

// Fills passed ack with data from the given packet. Note: ack must be freed
GBNAck get_ack_of_packet(GBNPacket packet);

// Increment window head 'n' packets
int inc_head(int n);

// Moves window head to given packet
int inc_head_to_packet(GBNPacket packet);

// Sets window size to 'size'
int set_win_size(int size);

// Copies given packet into buffer memory
int put_packet(GBNAck packet);

// Returns next packet in window, returns NULL if at the end of window
GBNPacket get_next_in_window(GBNPacket);

/*** end class RingBufferWindow ***/

#endif //ringbufferwindow_h
