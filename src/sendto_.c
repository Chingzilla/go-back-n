#include "sendto_.h"

void init_net_lib(double probability, unsigned int seed)
{
	if ((probability < 0) || (probability > 1)) {
		printf("%f : Error in setting packet lost probability!\n",probability);
		exit(1);
	}
	else {
		srand(seed);
		p_threshold = (int)(probability * 1000);
	}

}

int sendto_(int sock, void* message, int length, int flags, struct sockaddr* sa, int dest_len)
{
	int rnd;
	double rnd_max = (double)RAND_MAX;

	rnd = ((rand() / rnd_max) * 1000);
	if (rnd > p_threshold) {
		return sendto(sock, message, length, flags, sa, dest_len);
	}
    printf("Packet 'failed' to send\n");
	return length;
}
