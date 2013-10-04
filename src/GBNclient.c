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

int main(int argc, char *argv[]) {
    
            /* check command line args. */
            if(argc<7)
            {
            printf("usage : %s <server_ip> <server_port> <error rate> <random seed> <send_file> <send_log> \n", argv[0]);
            exit(1);
            }

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
            if ((int) size % (MAXDATASIZE) == 0){
                 num_packets = (int) size / (MAXDATASIZE);
            }
            else{
                num_packets = (int) size/(MAXDATASIZE) +1;
            }   

            // Create packetObj array in stack:
            GBNPacketObj data_packets[num_packets];
            int i;
            size_t retVal;
            for (i =0; i<num_packets;i++){
                if(i < num_packets-1){
                        retVal = fread(data_packets[i].data, sizeof(char), MAXDATASIZE, fd);
                        // Set the size
                       // data_packets[i].data[MAXDATASIZE]='\0';
                        data_packets[i].size = retVal ;
                        data_packets[i].seq_num =i;
                        if(retVal != MAXDATASIZE){
                            printf("Reading Error\n");
                            printf("%d\n", (int)retVal);
                        }
                }
                else{
                        // Last packet case:
                       size_t leftBytes = (int) size % MAXDATASIZE;
                        retVal = fread(data_packets[i].data, sizeof(char),leftBytes, fd);
                        //data_packets[i].data[leftBytes+1]='\0';
                         data_packets[i].size = retVal ;
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
            RingBufferWindow sender_window_buffer;
            int SWS = 9;
            int LAR = -1;
            int LFS = -1;

            int init_val = 200;
            if(rbw_init(&sender_window_buffer, init_val) != 0){
                 fprintf(stderr, "Error creating ringbuffer\n");
                 exit(1);
             }

             // Copy all the packets to the sender window buffer:
             for (i =0; i < num_packets; i++){
                 if(rbw_put_packet(sender_window_buffer, &data_packets[i]) != 0){
                             fprintf(stderr, "Error putting packet into ringbuffer\n");
                              exit(1);
                 }

                if(rbw_inc_head(sender_window_buffer, 1) !=0){
                        printf("Error increamenting head\n");
                }
             
             }

            // Set the ring buffer window head to start:
            sender_window_buffer->win_head = 0;

            // Get first packet to send:
            GBNPacket to_send, to_wait;
            to_send= rbw_get_packet_n(sender_window_buffer,0);

            // Initialize variables for select:
            fd_set rfds;
            int max_fd;
            int ret_select = 0;
            int recv_free_win_size = -1;

            int j=0;
            int first_entry = 1;

            int resend;
            double time_now, wait_time_millisecs;

            FD_ZERO(&rfds);
            FD_SET(sd, &rfds);

            GBNAck received_ack;
            received_ack = (GBNAck) malloc(sizeof(GBNAckObj));
       
            int ack_bytes;
           
            // Open log file:
            FILE* fd_log;
            fd_log = fopen(argv[6],"w");

            printf("Number of packets = %d\n",num_packets);

           while(LAR != num_packets ){
                        //printf("waiting in the main loop\n");
                        // Check to see if receiver advertised its free window size:
                        if((recv_free_win_size > 0) || (first_entry == 1)){
                                first_entry = 0;
                                //printf("Entered the send packet loop\n");
                                if(recv_free_win_size > 0){
                                    SWS = recv_free_win_size;
                                }
                                

                                // Within sender window size
                                while((LFS - (LAR+1))< (SWS-1)){
                                    // Send packets
                                    int bytesSent;
                                    //printf("Inside send loop\n"); 
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
                                        if(rbw_inc_head(sender_window_buffer, 1) !=0){
                                                 printf("Error increamenting head\n");
                                        }

                                        // Get the next packet to send:
                                        to_send = rbw_get_next_packet(sender_window_buffer, to_send);
                                    }
                                }
                         }
                        else if (recv_free_win_size == 0){
                            // Keep sending empty packet:
                             GBNPacket empty_packet;
                             empty_packet = (GBNPacket) malloc(sizeof(GBNPacketObj));

                             empty_packet->seq_num = LFS;
                             empty_packet->data[0]='\0';
                             int sent;
                             sent = send_packet(empty_packet ,sd, remoteServAddr);
                             if(sent <= 0){
                                 printf("Error sending packet\n");
                             }
                             fprintf(fd_log, "Sending empty packet to the server!!!\n");

                        }
                        

                        // Wait for Ack on LAR+1 from the server:
                        //to_wait = rbw_get_packet_n(sender_window_buffer, LAR+1);

                            //time_now = get_time_in_millisecs(); 
                            //if(time_now - to_wait->send_time <= 0){
                            //    printf("Now = %f",time_now);
                            //    printf("wait time  = %f",time_now - to_wait->send_time);
                            //    wait_time_millisecs = 50;
                            //}
                            //else{
                             //   wait_time_millisecs = 50 - time_now - (to_wait->send_time);

                            //}
                            //printf("%f\n",wait_time_millisecs);


                            struct timeval tv;
                            tv.tv_sec = 0.0;
                            tv.tv_usec =50*1000;
                           
                            ret_select = select(sd+1, &rfds, NULL, NULL, &tv);

                            if(ret_select == -1){
                                perror("select()"); 
                            }                            
                            else{

                                    if(FD_ISSET(sd, &rfds) != 0){
                                        printf("ACK available now\n");
                                        ack_bytes = get_ack(received_ack, sd, remoteServAddr);
                                        // if(ack_bytes <= 0){
                                        //     printf(" Error: %d bytes of ACK received\n", ack_bytes);
                                        // }

                                        // Log
                                        // Todo : add time
                                        fprintf(fd_log, "<Receive> <%d> <%d> <%d> <%d>\n", received_ack->seq_num, received_ack->rev_win_size, LAR, LFS);

                                        // Check if the received ack is a new one: 
                                        if(received_ack->seq_num > LAR){
                                                // Slide the LAR window forward:
                                                LAR = received_ack->seq_num;
                                                recv_free_win_size = received_ack->rev_win_size;
                                        }

                                          // Reset ack_bytes:
                                         ack_bytes = 0;          
                                    }
                                    else{
                                            printf("No ACK received. Need to resend data\n");
                                      
                                            for (j= LAR+1; j <= LFS; j++){
                                                    // Set the window head to LAR:
                                                   if(LAR < 0){
                                                        sender_window_buffer->win_head = 0;
                                                   }
                                                   else{
                                                        sender_window_buffer->win_head = LAR;
                                                   }

                                                    // Resend packets
                                                    to_send = rbw_get_packet_n(sender_window_buffer, j);
                                                    printf("Resending packet %d\n",to_send->seq_num);
                                                    resend = send_packet(to_send ,sd, remoteServAddr);
                                                    if(resend <= 0){
                                                        printf("Error resending data!!!\n");
                                                    }
                                                   
                                                    // Log
                                                    if (recv_free_win_size >0){
                                                         fprintf(fd_log, "<Resend> <%d> <%d> <%d> <%d> <%f>\n", to_send->seq_num, recv_free_win_size, LAR, LFS, to_send->send_time);
                                                     }else
                                                    {
                                                         fprintf(fd_log, "<Resend> <%d> <%d> <%d> <%f>\n", to_send->seq_num, LAR, LFS, to_send->send_time);
                                                    }
                                                    
                                                    // Increment the head of the ring buffer window:
                                                    if(rbw_inc_head(sender_window_buffer, 1) !=0){
                                                             printf("Error increamenting head\n");
                                                    }
                                                    
                                            }
                                    }
                            }
                    }

            fclose(fd_log);
            fclose(fd);

return 0;
}
