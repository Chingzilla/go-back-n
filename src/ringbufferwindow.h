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
} RingBufferWindow;

#endif //ringbufferwindow_h
