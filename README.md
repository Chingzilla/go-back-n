go-back-n
=========

Simple Server and Client application that using a Go Back N to transfer files


Authors
=======
Chris Ching
Jiwan Rana


Usage
=====
Server: +
    GBNserver <server_port> <error rate> <random seed> <send_file> <send_log>

Client: +
    GBNclient <server_ip> <server_port> <error rate> <random seed> <send_file> <send_log>


Building
========
cd src/ && make


Features
========

.Server:
* Drops out-of-range packets
* Ack with latest packet if packet received is older
* Working sliding window (using ringbuffer data structure)
* Uses accumulative acks
* Acks contain server's window size
* Window size determined by LPRead
* Packets are properly stored in window
* Writes packets to disk
* Logging
* Exits cleanly at end of file
* Correctly transfers files

.Client:
* Sends packets
* Adjusts window to server (flow control)
* Recovers for when rws = 0
* Limits packets re-send by storing sent state
* Resends packets after timeout of 50ms
* Exits on ether last frame ack or if packet is resent 10 times
* Logging


Object (like) Structures
========================
.RingBufferWindow
Packet data is stored in a ringbufferwindow struct for both the client and server. This allows us to
focus on the protocol, and not on memory management and sequence number wrapping. Buffer head is
used as ether LAR+1 or LFRevd;

.GBNPacket
Packet data is stored in a gbnpacket struct and is used for both sending and receiving. Please
see header files for further documentation

.GBNAck
Ack data is stored in a gbnack struct and are typically generated from a GBNPacket. Please see
header file for further documentation.


Protocol
========

Packets consist of a 2 byte header and up to 1024 bytes of file data. Head is simply the packet's sequence number.
Packet smaller then 1024, excluding the header, are considered the end of the file. All
packets in the sliding window are resent if no cumulative ack is received within 50ms.

Acks are 4 bytes long with the 1st 2 bytes being the sequence number (LAR) and 2nd 2 bytes being
the rws.

If RWS=0, the sender sends LAR+1 packet each timeout until a new ack is received.

Max sequence number is constant BUFFERSIZE in ringbuffer.h

Client Testing
==============
There is commented-out code that can be used to test the client's handling of edge cases

RWS=0
-----
Uncomment the relevant lines in GBNserver.c and compile. Instead of returning an accumulative ack
right after a packet was received, the server only acks writes data to files and acks after the rws = 0

Last ACK dropped
----------------
Uncomment the relevant lines in GBNserver.c and compile

Server will not ack the last packet of the file to the client. Client must recover form this
and exit cleanly.


Copyright
=========
Code found in this project is not to be used by anyone other then the original
authors for purpose of graded assignments in CSCI-4273 or CSCI-5273.
