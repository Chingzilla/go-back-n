/* GBNclient.c */
/* This is a sample UDP client/sender using "sendto_.h" to simulate dropped packets. */
/* This code will not work unless modified. */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */
#include <signal.h>
#include <unistd.h>
#include "sendto_.h"

#include "gobackn.h"
#include "ringbufferwindow.h"

const int EFFECTIVE_DATA_SIZE = MAXDATASIZE-1;

#define true 1

int main(int argc, char *argv[]) {
    
/* check command line args. */
// if(argc<7)
// {
// printf("usage : %s <server_ip> <server_port> <error rate> <random seed> <send_file> <send_log> \n", argv[0]);
// exit(1);
// }

/* Note: you must initialize the network library first before calling sendto_(). The arguments are the <errorrate> and <random seed> */
init_net_lib(atof(argv[3]), atoi(argv[4]));
printf("error rate : %f\n",atof(argv[3]));

/* socket creation */
int sd;
if ((sd = socket(AF_INET, SOCK_DGRAM,0)) < 0)
{
printf("%s: cannot create socket \n",argv[0]);
exit(1);
}

/* get server IP address (input must be IP address, not DNS name) */
struct sockaddr_in remoteServAddr;
bzero(&remoteServAddr,sizeof(remoteServAddr)); //zero the struct
remoteServAddr.sin_family = AF_INET; //address family
remoteServAddr.sin_port = htons(atoi(argv[2])); //sets port to network byte order
remoteServAddr.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address
printf("%s: sending data to '%s:%s' \n", argv[0], argv[1], argv[2]);

// Read in the data file:
printf("%s\n", argv[5]);
FILE* fd;
fd = fopen(argv[5],"rb");
if(fd == NULL){
printf("Error opening the send file\n");
exit(1);
}

// Get the size of the file to figure out the number of packets needed
long int size;
            fseek(fd, 0, SEEK_END); // seek to end of file
            size = ftell(fd); // get current file pointer
            fseek(fd, 0, SEEK_SET); // seek back to beginning of file
            printf("%ld \n",size);

            int num_packets;
            if ((int) size % (EFFECTIVE_DATA_SIZE) == 0){
                 num_packets = (int) size / (EFFECTIVE_DATA_SIZE);
            }
            else{
                num_packets = (int) size/(EFFECTIVE_DATA_SIZE) +1;
            }	

            // Create packetObj array in stack:
            GBNPacketObj data_packets[num_packets];
            int i;
            size_t retVal;
            for (i =0; i<num_packets;i++){
                if(i < num_packets-1){
                    // printf("Packet %d\n", i);
                        // Read the data
                        retVal = fread(data_packets[i].data, sizeof(char), EFFECTIVE_DATA_SIZE, fd);
                        // Set the size
                        data_packets[i].data[MAXDATASIZE]='\0';
                        data_packets[i].size = retVal +1;
                        data_packets[i].seq_num =i;
                        if(retVal != EFFECTIVE_DATA_SIZE){
                            printf("Reading Error\n");
                            printf("%d\n", (int)retVal);
                        }
                }
                else{
                        // Last packet case:
                    printf("Last packet \n");
                       size_t leftBytes = (int) size % EFFECTIVE_DATA_SIZE;
                        retVal = fread(data_packets[i].data, sizeof(char),leftBytes, fd);
                        data_packets[i].data[leftBytes+1]='\0';
                         data_packets[i].size = retVal +1;
                         data_packets[i].seq_num = i;
                        if(retVal != leftBytes){
                            printf("Reading Error\n");
                            printf("%d\n", (int)retVal);
                        }
                }
                // printf("%d\n",(int) retVal);
                // Reset retval:
                retVal = 0;
            }
            
            // Initialize sender window buffer, LAR, LFS:
            RingBufferWindow sender_window_bufer;
           int SWS = 6;
    int LAR = -1;
    int LFS = -1;

    int init_val = 200;
    if(rbw_init(&sender_window_bufer, init_val) != 0){
         fprintf(stderr, "Error creating ringbuffer\n");
         exit(1);
     }

     // Copy all the packets to the sender window buffer:
     for (i =0; i < num_packets; i++){
                        printf("%d\n", i);
     if(rbw_put_packet(sender_window_bufer, &data_packets[i]) != 0){
     fprintf(stderr, "Error putting packet into ringbuffer\n");
                                  exit(1);
     }
                        // else{
                        // printf("Success putting packet %d to the buffer\n",i );
                        // }
                    if(rbw_inc_head(sender_window_bufer, 1) !=0){
                        printf("Error increamenting head\n");
                    }
                    // else{
                    // printf(" Head incremented to %d \n", i+1);
                    // }
     }

        // Set the ring buffer window head to start:
        sender_window_bufer->win_head = 0;
        printf("%d\n", sender_window_bufer->win_head);

        // Get first packet to send:
         GBNPacket to_send, to_wait;
         to_send= rbw_get_packet_n(sender_window_bufer,0);

         printf("%d\n", to_send->seq_num);


                // Initialize variables for select:
            fd_set rfds;
            int ret_select;
            int recv_free_win_size = -1;

            int j=0;

            int resend;

            FD_ZERO(&rfds);
            FD_SET(sd, &rfds);

            GBNAck received_ack;
            received_ack = (GBNAck) malloc(sizeof(GBNAckObj));
            
            int ack_bytes;
           
            // Open log file:
            FILE* fd_log;
            fd_log = fopen(argv[6],"w");

           while(LAR != num_packets ){
                        // Check to see if receiver advertised its free window size:
                        if(recv_free_win_size > 0){
                            SWS = recv_free_win_size;
                        }

                        // Within sender window size
                        while((LFS - (LAR+1) )< SWS){
                            // Send packets
                            int bytesSent;
                           
                            bytesSent = send_packet(to_send ,sd, remoteServAddr);
                            if(bytesSent <= 0){
                                 printf("Error sending packet!\n");
                            }
                            else{
                                // Log
                                if (recv_free_win_size >0){
                                     fprintf(fd_log, "<Send> <%d> <%d> <%d> <%d> <%f> \n", to_send->seq_num, recv_free_win_size, LAR, LFS, to_send->send_time);
                                }else
                                {
                                     fprintf(fd_log, "<Send> <%d> <%d> <%d> <%f>\n", to_send->seq_num, LAR, LFS, to_send->send_time);
                                }
                               
                                //Packet sent:
                                // Increment the LFS:
                                LFS +=1;

                                // Increment the head of the ring buffer window:
                                if(rbw_inc_head(sender_window_bufer, 1) !=0){
                                         printf("Error increamenting head\n");
                                }

                                // Get the next packet to send:
                                to_send = rbw_get_next_packet(sender_window_bufer, to_send);
                            }
                        }

                        // Wait for Ack on LAR+1 from the server:
                            to_wait = rbw_get_packet_n(sender_window_bufer, LAR+1);

                            double now_millisecs = get_time_in_millisecs();
                            double wait_time_millisecs = to_wait->send_time + 50 - now_millisecs;
                            printf ("%f\n",wait_time_millisecs);

                            struct timeval tv;
                            tv.tv_sec = 0;
                            tv.tv_usec = wait_time_millisecs * 1000;

                            ret_select = select(sd+1, &rfds, NULL, NULL, &tv);
                            if(FD_ISSET(sd, &rfds) == 1){
                                    printf("ACK available now\n");
                                    ack_bytes = get_ack(received_ack, sd, remoteServAddr);
                                    if(ack_bytes <= 0){
                                        printf(" Error: %d bytes of ACK received\n", ack_bytes);
                                    }

                                    // Log
                                    // Todo : add time
                                    fprintf(fd_log, "<Receive> <%d> <%d> <%d> <%d>\n", received_ack->seq_num, received_ack->rev_win_size, LAR, LFS);

                                    // Check if the received ack matches the seq num of LAR+1:
                                    if(received_ack->seq_num = to_wait->seq_num){
                                            // Slide the LAR window forward:
                                            LAR += 1;
                                    }

                                    // Set the value of receivers free window size:
                                    recv_free_win_size = received_ack->rev_win_size;
                                      // Reset ack_bytes:
                                     ack_bytes = 0;          
                            }
                             else{
                                    printf("No ACK received. Need to resend data\n");
                              
                                    for (j= LAR+1; j <= LFS; j++){
                                           to_send = rbw_get_packet_n(sender_window_bufer, j);
                                          resend = send_packet(to_send ,sd, remoteServAddr);
                                          if(resend <= 0){
                                            printf("Error resending data!!!\n");
                                          }
                                        // Log
                                        if (recv_free_win_size >0){
                                             fprintf(fd_log, "<Send> <%d> <%d> <%d> <%d> <%f>\n", to_send->seq_num, recv_free_win_size, LAR, LFS, to_send->send_time);
                                         }else
                                        {
                                             fprintf(fd_log, "<Send> <%d> <%d> <%d> <%f>\n", to_send->seq_num, LAR, LFS, to_send->send_time);
                                        }
                            }

                    }
            }

            fclose(fd_log);
            fclose(fd);

// while (within sender window size)
// send packets


// select( sd, read, NULL,NULL, &timeval of the LAR +1 packet);
// if (ack received) and RWS >0
// slide the window forward

// else if (RWS == 0)
// handle the corner case

// else data is not received at all until timeout
// so resend all packets from LAR+1 to LFS.

return 0;
}
