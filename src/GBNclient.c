/* GBNclient.c */
/* This is a sample UDP client/sender using "sendto_.h" to simulate dropped packets.  */
/* This code will not work unless modified. */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>   /* memset() */
#include <sys/time.h> /* select() */
#include <signal.h>
#include <unistd.h>
#include "sendto_.h"

#include <time.h>

#include "gobackn.h"
#include "ringbufferwindow.h"

FILE *logfile;

int logevent(char *event, int seq_num, int free_slots, int lar, int lfs){

    time_t current_time = time(NULL);
    char *strtime = ctime(&current_time);
    strtime[strlen(strtime) - 1] = '\0';
    char strbuf[100];
    
    if(free_slots >= 0){
        sprintf(strbuf, "%s: <%s> <seq:%d> [free:%d] <LAR:%d> <LFS:%d>\n",
          strtime, event, seq_num, free_slots, lar, lfs);
    } else {
        sprintf(strbuf, "%s: <%s> <seq:%d> <LAR:%d> <LFS:%d>\n",
          strtime, event, seq_num, lar, lfs);
    }
    printf(strbuf);
    fprintf(logfile, strbuf);
    fflush(logfile);
    return 0;
}

// Copy data from file and put in packet. Returns number of bytes writen
int file_to_packet(FILE *fin, GBNPacket packet){
    int nbytes = fread(packet->data, sizeof(char), MAXDATASIZE, fin);
    packet->size = nbytes;
    
    printf("read %d from file\n", nbytes);
    return nbytes;
}

#define TIMEOUT 500 // TODO: change to 50
int get_timeout(struct timeval start, struct timeval *timeout){
    struct timeval current;
    gettimeofday(&current, NULL);

    double diff;

    diff = (current.tv_sec - start.tv_sec) * 1000 + ( current.tv_usec - start.tv_usec);

    if( diff > TIMEOUT ){
        timeout->tv_sec = 0;
        timeout->tv_usec = 0;
    }else{
        timeout->tv_sec = 0;
        timeout->tv_usec = (long)(TIMEOUT - diff);
    }
    printf("Timeout: %d\n", timeout->tv_usec);
    return 0;
}

int main(int argc, char *argv[]) {
    
    /* check command line args. */
    if(argc<7)
    {
        printf("usage : %s <server_ip> <server_port> <error rate> <random seed> <send_file> <send_log> \n", argv[0]);
        exit(1);
    }

    /* Note: you must initialize the network library first before calling sendto_().  The arguments are the <errorrate> and <random seed> */
    init_net_lib(atof(argv[3]), atoi(argv[4]));
    printf("error rate : %f\n",atof(argv[3]));

    /* socket creation */
    int sd;
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) <0)
    {
        printf("%s: cannot create socket \n",argv[0]);
        exit(1);
    }

    /* get server IP address (input must be IP address, not DNS name) */
    struct sockaddr_in remoteServAddr;
    bzero(&remoteServAddr,sizeof(remoteServAddr));               //zero the struct
    remoteServAddr.sin_family = AF_INET;                 //address family
    remoteServAddr.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
    remoteServAddr.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address
    printf("%s: sending data to '%s:%s' \n", argv[0], argv[1], argv[2]);

    // Open logfile
    logfile = fopen(argv[6], "a");
        if (logfile == NULL){
            fprintf(stderr, "Error opening logfile: %s\n", argv[6]);
            exit(1);
        }
    fprintf(logfile, "*** Starting Server ***\n");
    fflush(logfile);

    // Open file to send
    FILE *fd = fopen(argv[5], "rb");
    if (fd == NULL){
        fprintf(stderr, "Error opening file %s\n", argv[5]);
        exit(1);
    }
    int fd_eof = 0;

    // Get file size
    fseek(fd, 0, SEEK_END);
    int file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    
    printf("File %s is %d bytes long\n", argv[5], file_size);
    
    // Setup ringbuffer
    RingBufferWindow win_buff;
    int sws = 9; // Per assignment
    int lfs;
    int free_space = 9;

    rbw_init(&win_buff, sws);

    GBNAck lar = malloc(sizeof(GBNAckObj));
    bzero(lar, sizeof(GBNAckObj));
    GBNPacket tmp_packet;

    // Timeout stuff
    fd_set rfds;

    // Fill window with data from file
    for(int i=0; i < sws; i++){
        tmp_packet = rbw_get_packet_n(win_buff, i);
        file_to_packet(fd, tmp_packet);
        if( tmp_packet->size < MAXDATASIZE ){
            fd_eof = 1;
            sws = i;
            break;
        }
    }

    // Send packets
    while(1){
        // Send Packets in window
        for(int i=0; i < sws; i++){
            tmp_packet = rbw_get_packet_n(win_buff, i);
            if(tmp_packet->recvd){
                continue;
            }
            
            // Check if packet has been sent before
            char *strevent;
            if(tmp_packet->send_time.tv_sec){
                strevent = "Resend";
            }else{
                strevent = "Send";
            }
            send_packet(tmp_packet, sd, remoteServAddr);
            logevent(strevent, tmp_packet->seq_num, -1, -1, lfs);
            lfs = i;
        }

        // Check for ack loop timeout
        struct timeval time_out;
        tmp_packet = rbw_get_packet_n(win_buff, 0);
        get_timeout(tmp_packet->send_time, &time_out);

        FD_ZERO(&rfds);
        FD_SET(sd, &rfds);
        select(sd+1, &rfds, NULL, NULL, &time_out);

        if(FD_ISSET(sd, &rfds) != 0){
            int lar_cache = lar->seq_num;
            get_ack(lar, sd, remoteServAddr);
            logevent("Receive", lar->seq_num, lar->rev_win_size, lar_cache, lfs);
            sws = lar->rev_win_size;

            //TODO: Update window and buffer
            int ack_n = rbw_get_n_ack(win_buff, lar);
            if(ack_n < 0){
                printf("ack is behind the current LAR, ignoring\n");
                continue;
            }
            //TODO read more packets from file
            
            rbw_inc_head(win_buff, ack_n + 1); // Move head to LAR + 1
            rbw_set_win_size(win_buff, lar->rev_win_size);
        }else{
            for(int i=0; i < sws; i++){
                rbw_get_packet_n(win_buff, i)->recvd = 0;
            }
        }
    }
    fclose(logfile);
    fclose(fd);
}
