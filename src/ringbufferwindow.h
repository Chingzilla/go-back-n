/*** src/ringbufferwindow.h
* Authors: Chris Ching and Jiwan Rana
***/

#ifndef ringbufferwindow_h
#define ringbufferwindow_h

#include "gobackn.h"

//TODO: Add ring buffer window
#define BUFFSIZE 100

// Return sequence number of given value (using mod)
int get_seq_num(int);

// Returns diffrents between given values, assumes n2 is after n1
int get_seq_diff(int n1, int n2);

// Returns sequence number of seq_num(n1) + n2, respects buffer boundaries
int seq_add(int n1, int n2);

/** class RingBufferWindow **/
typedef struct __ringbufferwindow {
    GBNPacket buffer[BUFFSIZE];
    int win_size;
    int win_head;
} RingBufferWindowObj, *RingBufferWindow;

// Initialize RingBufferWindow
int rbw_init(RingBufferWindow *pself, int win_size);

// Get packet in buffer matching (seq number)
// Returns packet that is at offset 'n' from window head
GBNPacket rbw_get_packet_n(RingBufferWindow self, int n);

// Returns next packet based on the given packet's sequence number
GBNPacket rbw_get_next_packet(RingBufferWindow self, GBNPacket packet);

// Fills passed ack with proper information of the packet offset 'n' from window head
int rbw_get_ack_n(RingBufferWindow self, int n, GBNAck ack);

// Fills passed ack with data from the given packet.
int rbw_get_ack_of_packet(RingBufferWindow self, GBNAck ack, GBNPacket packet);

// Increment window head 'n' packets
int rbw_inc_head(RingBufferWindow self, int n);

// Moves window head to given packet
int rbw_inc_head_to_packet(RingBufferWindow self, GBNPacket packet);

// Sets window size to 'size'
int rbw_set_win_size(RingBufferWindow self, int size);

// Copies given packet into buffer memory
int rbw_put_packet(RingBufferWindow self, GBNPacket packet);

// Returns next packet in window, returns NULL if at the end of window
GBNPacket rbw_get_next_in_window(RingBufferWindow self, GBNPacket);

/*** end class RingBufferWindow ***/

#endif //ringbufferwindow_h