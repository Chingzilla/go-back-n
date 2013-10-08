/* GBNserver.c */
/* This is a sample UDP server/receiver program */
/* This code will not work unless modified. */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <stdlib.h>
#include <time.h>
#include "sendto_.h"
#include "gobackn.h"
#include <fcntl.h>
#include "ringbufferwindow.h"

#include <time.h>

FILE *logfile;

int logevent(char *event, int seq_num, int free_slots, int lfread, int lfrcvd, int lfa){

    char strbuf[100];
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    
    if(free_slots >= 0){
        sprintf(strbuf, "%d.%d: <%s> <seq:%d> [free:%d] <LFRead:%d> <LFRcvd:%d> <LAF:%d>\n",
          curtime.tv_sec, curtime.tv_usec, event, seq_num, free_slots, lfread, lfrcvd, lfa);
    } else {
        sprintf(strbuf, "%d.%d: <%s> <seq:%d> <LFRead:%d> <LFRcvd:%d> <LAF:%d>\n",
          curtime.tv_sec, curtime.tv_usec, event, seq_num, lfread, lfrcvd, lfa);
    }
    printf(strbuf);
    fprintf(logfile, strbuf);
    fflush(logfile);
    return 0;
}

int main(int argc, char *argv[]) {

    /* check command line args. */
    if(argc<6) {
        printf("usage : %s <server_port> <error rate> <random seed> <send_file> <send_log> \n", argv[0]);
        exit(1);
    }
    printf("error rate : %f\n",atof(argv[2]));

    /* Note: you must initialize the network library first before calling sendto_().  The arguments are the <errorrate> and <random seed> */
    init_net_lib(atof(argv[2]), atoi(argv[3]));
    printf("error rate : %f\n",atof(argv[2]));

    /* open log file */
    logfile = fopen(argv[5], "a");
        if (logfile == NULL){
            fprintf(stderr, "Error opening logfile: %s\n", argv[5]);
            exit(1);
        }
    fprintf(logfile, "*** Starting Server ***\n");
    fflush(logfile);

    /* open output file */
    FILE *outf;
    outf = fopen(argv[4], "w");
        if(outf == NULL){
            fprintf(stderr, "Error creating output file: %s\n", argv[4]);
            exit(1);
        }

    /* socket creation */
    int sock;

    // Open socketf and bind
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        fprintf(stderr, "server: error getting socket\n");
        exit(1);
    }

    /* bind server port to "well-known" port whose value is known by the client */
    struct sockaddr_in servAddr, clientAddr;
    bzero(&servAddr,sizeof(servAddr));                    //zero the struct
    servAddr.sin_family = AF_INET;                   //address family
    servAddr.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
    servAddr.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine
    if(bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))<0)
    {
        printf("%s: cannot to bind port number %s \n",argv[0], argv[1]);
        exit(1); 
    }

    printf("Listening on port %s...\n", argv[1]);

    // Get file
    RingBufferWindow recv_win;
    if(rbw_init(&recv_win, 6 ) != 0){
        fprintf(stderr, "Error creating ringbuffer\n");
        exit(1);
    }

    int lf_read = 0;
    int rws = 6;
    int laf = rws - 1;
    int free_space = rws;

    GBNPacket tmp_packet;
    tmp_packet = malloc(sizeof(GBNPacketObj));
    int tmp_packet_n = 0;

    GBNAck ack;
    ack = malloc(sizeof(GBNAckObj));

    int addr_len;

    while(1){
        // Get packet
        //bzero(&clientAddr, sizeof(clientAddr));
        addr_len = sizeof(clientAddr);
        get_packet(tmp_packet, sock, &clientAddr, &addr_len);
        printf("Got Packet seq: %d size: %d client:%s:%d\n", tmp_packet->seq_num, tmp_packet->size,
          inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        logevent("Receive", tmp_packet->seq_num, -1, lf_read, 0, rws);

        // Check if packet is in window
        tmp_packet_n = rbw_get_n(recv_win, tmp_packet);
        if( tmp_packet_n < 0){
            // packet behind window, sending current ack
            rbw_get_ack_n(recv_win, -1, ack);

            logevent("Resend", ack->seq_num, free_space, lf_read, 0, rws);
            send_ack(ack, sock, clientAddr, addr_len);
            continue;
        }else if(tmp_packet_n >= rws){
            printf("Packet %d is out of window, dropping\n", tmp_packet->seq_num);
            continue;
        }

        // Put packet into buffer and check if packet was new
        if(rbw_put_packet(recv_win, tmp_packet) == 0){
            free_space -= 1;
        }

        // Check if we can ack
        GBNPacket next_packet = rbw_get_packet_n(recv_win, 0);
        int i;
        for(i=0; i < rws; ++i){
            // Check if packet has been received
            if(next_packet->recvd != 1){
                break;
            }

            // Get next packet and check if it in NULL (out of window)
            next_packet = rbw_get_next_packet(recv_win, next_packet);
            if (next_packet == NULL){
                printf("Warning: reached end of window unexpectedly\n");
                break;
            }
        }

        // Check head of window was received
        if(i == 0){
            continue;
        }

        // Slide window and ack
        lf_read -= i;
        rws -= i;
        free_space += i;
        rbw_inc_head(recv_win, i);
        rbw_set_win_size(recv_win, rws);

        /**** un-comment to test no last ack ****/
        GBNPacket ack_packet = rbw_get_packet_n(recv_win, -1);
        //if(ack_packet->size == MAXDATASIZE){ // comment to disable last ack drop test
        rbw_get_ack_n(recv_win, -1, ack);
        logevent("Send", ack->seq_num, free_space, lf_read, 0, rws);
        send_ack(ack, sock, clientAddr, addr_len);
        //} // comment to disable last ack drop test

        // Write data to file
        GBNPacket packet_to_read;
        while(lf_read < 0){
            packet_to_read = rbw_get_packet_n(recv_win, lf_read);
            printf("Writing Packet seq: %d size: %d\n", packet_to_read->seq_num, packet_to_read->size);
            fwrite(packet_to_read->data, 1, packet_to_read->size, outf);
            rws++; // RWS is now bigger
            lf_read++;
        }
        //lf_read = -1; // reset LFRead

        // Exit if size is < max size
        // TODO detect end of file
        if(packet_to_read->size != MAXDATASIZE){
            printf("Read the end of file!\n");
            break;
        }
    }

    fclose(logfile);
    fclose(outf);
}

