go-back-n
=========

Simple Server and Cilent application that using a Go Back N to tranfer files

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
* Ack with latest packet if packet is older
* Working sliding window
* Uses accumulative acks
* Acks contain server's window size
* Window size determined by LPRead
* Packets are properly stored in window
* Writes packets to disk
* Logging
* Exits cleanly at end of file

.Client:
* Sends packets
* Adjusts window to server (flow control)
* Recovers for when window size = 0
* Limits packets send by storing sent state
* Resends packets after timeout of 50ms
* Logging

Known Issues
============
1. client segfaults on break of main loop. Really weird bug where break or exit() causes the
    program to segfult

Copyright
=========
Code found in this project is not to be used by anyone other then the original
authors for purpose of graded assignments in CSCI-4273 or CSCI-5273.
