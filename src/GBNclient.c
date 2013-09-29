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

	// Initialize the ring buffer
	

	// Read in the data file: 
	FILE* fd_send_file;
	fd_send_file = fopen(argv[5],"rb");

	


	// Generate packets and put into the ring buffer


	// while (within sender window size)
		// send packets


	// select( sd, read, NULL,NULL, &timeval of the LAR +1 packet);
	 	// if (ack received) and RWS >0 
		// slide the window forward

		// else if (RWS == 0)
		//  handle the corner case

		// else data is not received at all until timeout
		// so resend all packets from LAR+1 to LFS.



	/* Call sendto_ in order to simulate dropped packets */
	int nbytes;
	char msg[] = "send this";
	unsigned int remote_length;
	nbytes = sendto_(sd,msg, strlen(msg),0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
	if (nbytes > 0){
		printf("Sent first message\n");
	}

	return 0;
}
