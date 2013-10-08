go-back-n
=========

Simple Server and Client application that using a Go Back N to transfer files

Usage
=====
Server: +
    GBNserver <server_port> <error rate> <random seed> <send_file> <send_log>

Client: +
    GBNclient <server_ip> <server_port> <error rate> <random seed> <send_file> <send_log>

Building
========
cd src/ && make

Working
=======

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

Copyright
=========
Code found in this project is not to be used by anyone other then the original
authors for purpose of graded assignments in CSCI-4273 or CSCI-5273.
