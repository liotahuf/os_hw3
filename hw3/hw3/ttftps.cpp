include "ttftps.h"




int main(int argc, char* argv[])
{
	struct sockaddr_in my_addr = { 0 };
	struct sockaddr_in client_addr = { 0 };
	char buffer[MAX_BYTES];

	if (argc < 2) {
		fprintf(stderr, "ERROR: no port provided, please write: ./ttftps <Port #>\n");
		exit(1);
	}

	int server_socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_socketfd < 0)
	{
		fprintf(stderr, "ERROR opening socket\n");
		exit(1);
	}

	int portno = atoi(argv[1]);
	/* Zero out structure */
	memset(&my_addr, 0, sizeof(my_addr));

	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(portno);

	if (bind(server_socketfd, (struct sockaddr*) & my_addr, sizeof(my_addr)) < 0)
	{
		fprintf(stderr, "ERROR on binding\n");
		exit(1);
	}

	for (;;)
	{
		unsigned int client_addr_len = sizeof(client_addr);

		int recvMsgSize = recvfrom(server_socketfd, buffer, MAX_BYTES, 0, (struct sockaddr*) & client_addr, &client_addr_len);
		if (recvMsgSize < 0)
		{
			fprintf(stderr, "TTFTP_ERROR: recvfrom - error occurred\n");
			emptySocket(server_socketfd);
			exit(1);
		}

	}



	const int WAIT_FOR_PACKET_TIMEOUT = 3;
	const int NUMBER_OF_FAILURES = 7;
	do
	{
		do
		{

			do
			{
				// TODO: Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
				// for us at the socket (we are waiting for DATA)

				if ()// TODO: if there was something at the socket and
				// we are here not because of a timeout
				{
					// TODO: Read the DATA packet from the socket (at
					// least we hope this is a DATA packet)
				}
				if (...) // TODO: Time out expired while waiting for data
				 // to appear at the socket
				{
					//TODO: Send another ACK for the last packet
					timeoutExpiredCount++;
				}

				if (timeoutExpiredCount >= NUMBER_OF_FAILURES)
				{
					// FATAL ERROR BAIL OUT
				}

			} while (...) // TODO: Continue while some socket was ready
			// but recvfrom somehow failed to read the data

				if (...) // TODO: We got something else but DATA
				{
					// FATAL ERROR BAIL OUT
				}
			if (...) // TODO: The incoming block number is not what we have
			// expected, i.e. this is a DATA pkt but the block number
			// in DATA was wrong (not last ACK’s block number + 1)
			{
				// FATAL ERROR BAIL OUT
			}
		} while (FALSE);
		timeoutExpiredCount = 0;
		lastWriteSize = fwrite(...); // write next bulk of data
	   // TODO: send ACK packet to the client
	} while (...); // Have blocks left to be read from client (not end of transmission)


	return 0;
}