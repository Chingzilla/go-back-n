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

#include "gobackn.h"
#include "ringbufferwindow.h"

const int EFFECTIVE_DATA_SIZE = MAXDATASIZE-1;

#define true 1

int main(int argc, char *argv[]) {
    
	/* check command line args. */
	// if(argc<7)
	// {
	// 	printf("usage : %s <server_ip> <server_port> <error rate> <random seed> <send_file> <send_log> \n", argv[0]);
	// 	exit(1);
	// }

	/* Note: you must initialize the network library first before calling sendto_().  The arguments are the <errorrate> and <random seed> */
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
	bzero(&remoteServAddr,sizeof(remoteServAddr));               //zero the struct
	remoteServAddr.sin_family = AF_INET;                 //address family
	remoteServAddr.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
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
            // printf("%ld \n",size);

            int num_packets;
            if ((int) size % (EFFECTIVE_DATA_SIZE) == 0){
                 num_packets = (int) size / (EFFECTIVE_DATA_SIZE);
                  // printf("%d\n", num_packets);
            }
            else{
                num_packets = (int) size/(EFFECTIVE_DATA_SIZE) +1;
                // printf("%d\n", num_packets);
            }	

            GBNPacket data_packets[num_packets];
              // Allocate memory for packets
            int i;
            for (i=0; i<num_packets; i++){
                data_packets[i] = (GBNPacket) malloc(sizeof(GBNPacket));
                 memset(data_packets[i],'\0',sizeof(GBNPacketObj)); // Free the memory of the packet 
            }

            // Open the send file and create packets
            size_t retVal;
            for (i =0; i<num_packets;i++){
                if(i < num_packets-1){              
                        retVal = fread(data_packets[i]->data, 1, EFFECTIVE_DATA_SIZE, fd);
                        data_packets[i]->data[MAXDATASIZE]='\0';
                        if(retVal != EFFECTIVE_DATA_SIZE){
                            printf("Reading Error\n");
                            printf("%d\n", (int)retVal);
                        }
                }
                else{
                        // Last packet case:
                       size_t leftBytes = (int) size % EFFECTIVE_DATA_SIZE;
                        retVal = fread(data_packets[i]->data, 1,leftBytes, fd);
                        data_packets[i]->data[leftBytes+1]='\0';                  
                        if(retVal != leftBytes){
                            printf("Reading Error\n");
                            printf("%d\n", (int)retVal);
                        }
                }   
            }

            // Initialize sender window buffer, LAR, LFS:
            RingBufferWindow sender_window_bufer;
   	const int SWS = 6;
   	int LAR = -1;
   	int LFS = -1;

   	int init_val = 200;
   	 if(rbw_init(&sender_window_bufer, init_val) != 0){
        		fprintf(stderr, "Error creating ringbuffer\n");
        		exit(1);
    	}

    	// Copy all the packets to the sender window buffer:
    	for (i =0; i < num_packets; i++){
    		if(rbw_put_packet(sender_window_bufer, data_packets[i]) != 0){
    			fprintf(stderr, "Error putting packet into ringbuffer\n");
    		}
    	}

    	// // Test creation of packets
    	// for(i=0; i<num_packets;i++){
    	// 	printf("%d\n", data_packets[i]->seq_num);
    	// }
    	

    	while(true){


    	}
	// while (within sender window size)
		// send packets


	// select( sd, read, NULL,NULL, &timeval of the LAR +1 packet);
	 	// if (ack received) and RWS >0 
		// slide the window forward

		// else if (RWS == 0)
		//  handle the corner case

		// else data is not received at all until timeout
		// so resend all packets from LAR+1 to LFS.



	// /* Call sendto_ in order to simulate dropped packets */
	// int nbytes;
	// char msg[] = "send this";
	// unsigned int remote_length;
	// nbytes = sendto_(sd,msg, strlen(msg),0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
	// if (nbytes > 0){
	// 	printf("Sent first message\n");
	// }

	return 0;
}
