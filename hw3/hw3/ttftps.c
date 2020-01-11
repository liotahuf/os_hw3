#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define FILE_NAME_SIZE 255
#define DATA_SIZE 512
#define HEADER_SIZE 4
#define MAX_SIZE 516
#define WRQ_OPCODE 2
#define DATA_OPCODE 3

void clear_socket(int socket);

struct WRQ_pck {
	unsigned short opcode;
	char filename[FILE_NAME_SIZE];
	char mode[FILE_NAME_SIZE];
	
} __attribute__((packed));

struct ACK_pck {
	unsigned short opcode;
	unsigned short block_num;
} __attribute__((packed));

struct DATA_pck {
	unsigned short opcode;
	unsigned short block_num;
	char data[DATA_SIZE];
} __attribute__((packed));



//********************************************
// function name:main 
// Description: implemetns a FFFTP Sever
// Parameters: port number to listen
// Returns: int(0 - sucssess, 1- failure)
//*********************************************
int main(int argc, char* argv[])
{
	struct sockaddr_in my_addr = { 0 };
	struct sockaddr_in client_addr = { 0 };
	struct WRQ_pck my_WRQ_pck;
	char buffer[MAX_SIZE];
	char filename[FILE_NAME_SIZE] = { 0 };
	char mode[FILE_NAME_SIZE] = { 0 };
	int timeoutExpiredCount;
	struct ACK_pck my_ACK_pck;
	struct DATA_pck my_DATA_pck;
	int received_bytes;
	bool end_transfer = false;
	

	if (argc < 2) {
		perror("TTFTP_ERROR: no port provided, please write: ./ttftps <Port #>\n");
		exit(1);
	}

	int server_socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_socketfd < 0)
	{
		perror("TTFTP_ERROR: opening socket\n");
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
		perror("TTFTP_ERROR: on binding\n");
		exit(1);
	}

	for (;;)
	{
		unsigned int client_addr_len = sizeof(client_addr);

		int recvMsgSize = recvfrom(server_socketfd, buffer, MAX_SIZE, 0, (struct sockaddr*) & client_addr, &client_addr_len);
		if (recvMsgSize < 0)
		{
			perror("TTFTP_ERROR: recvfrom - error occurred\n");
			clear_socket(server_socketfd);
			close(server_socketfd);
			exit(1);
		}
		my_WRQ_pck.opcode = *(short*)buffer;
		strcpy(my_WRQ_pck.filename, (buffer+2));
		strcpy(my_WRQ_pck.mode,buffer + strlen(buffer) + 1);
		if (ntohs(my_WRQ_pck.opcode) != WRQ_OPCODE)
		{
			perror("TTFTP_ERROR: Error in recvfrom - opcode not of WRQ\n");
			printf("RECVFAIL\n");
			clear_socket(server_socketfd);
			continue;
		}
		if (strcmp(mode, "octet") != 0)
		{
			perror("TTFTP_ERROR: Error in recvfrom - mode is not octet\n");
			printf("RECVFAIL\n");
			clear_socket(server_socketfd);
			continue;
		}
		printf("RECVOK\n");
		printf("IN:WRQ,%s,%s WRQ\n", filename, mode);
		FILE* fp = fopen(filename, "w");
		if (fp == NULL)
		{
			perror("TTFTP_ERROR: Error in opening file\n");
			clear_socket(server_socketfd);
			close(server_socketfd);
			exit(1);
		}

		int ack_count = 0;
		my_ACK_pck.opcode = htons(4);
		my_ACK_pck.block_num = htons(ack_count);
		if (sendto(server_socketfd, (void*)&my_ACK_pck, sizeof(my_ACK_pck), 0, (struct sockaddr*) &client_addr, &client_addr_len) == -1)
		{
			perror("TTFTP_ERROR:Error in sendto \n");
			fclose(fp);
			clear_socket(server_socketfd);
			close(server_socketfd);
			exit(1);
		}
		printf("OUT:ACK,0\n");

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

					fd_set read_fds;
					struct timeval timeOut = { 0 };
					timeOut.tv_sec = WAIT_FOR_PACKET_TIMEOUT;
					timeOut.tv_usec = 0;
					FD_ZERO(&read_fds);
					FD_SET(server_socketfd, &read_fds);
					int check_socket = select(server_socketfd + 1, &read_fds, NULL, NULL, &timeOut);
					
					if (check_socket>0)// TODO: if there was something at the socket and
					// we are here not because of a timeout
					{
						// TODO: Read the DATA packet from the socket (at
						// least we hope this is a DATA packet)
						received_bytes = recvfrom(server_socketfd, &my_DATA_pck, DATA_SIZE + HEADER_SIZE, 0, (struct sockaddr*) & client_addr, &client_addr_len);
						
						
						//printf("size data %d\n", (int)strlen(myData.data));
						if (received_bytes < 0)
						{
							perror("TTFTP_ERROR:Error in recvfrom\n");
							fclose(fp);
							clear_socket(server_socketfd);
							close(server_socketfd);
							exit(1);
						}
						
						
						break;
					}
					if (check_socket ==0) // TODO: Time out expired while waiting for data
					 // to appear at the socket
					{
						//TODO: Send another ACK for the last packet
						printf("FLOWERROR: waiting for data..\n");
						if (sendto(server_socketfd, (void*)&my_ACK_pck, sizeof(my_ACK_pck), 0, (struct sockaddr*) & client_addr, client_addr_len) < 0)
						{
							perror("TTFTP_ERROR: Error in sendto\n");
							fclose(fp);
							clear_socket(server_socketfd);
							close(server_socketfd);
							exit(1);
						}
						printf("OUT:ACK,%d\n", my_ACK_pck.block_num );
						timeoutExpiredCount++;
						
					}

					if (timeoutExpiredCount >= NUMBER_OF_FAILURES)// end transfer when timeout expired
					{
						printf("FLOWERROR: Time out\n");
						end_transfer = true;
						break;
					}

				} while (received_bytes < 0); // TODO: Continue while some socket was ready
				// but recvfrom somehow failed to read the data

				if (end_transfer)//end transfer when timeout expired
				{
					break;
				}
				if (ntohs(my_DATA_pck.opcode) != DATA_OPCODE) // TODO: We got something else but DATA
				{
					// FATAL ERROR BAIL OUT
					printf("FLOWERROR: pakcket's opcode wrong.\n");
					end_transfer = true;
				}
				if (ntohs(my_DATA_pck.block_num) != ack_count+1) // TODO: The incoming block number is not what we have
				// expected, i.e. this is a DATA pkt but the block number
				// in DATA was wrong (not last ACK�s block number + 1)
				{
					{
						// FATAL ERROR BAIL OUT
						printf("FLOWERROR: blockNum of data different from last ACK's blockNum\n");
						end_transfer = true;
					}
				}
			} while (false);
			timeoutExpiredCount = 0;
			if (end_transfer)//end transfer when timeout expired
			{
				break;
			}
			printf("IN:DATA %d %d\n",ntohs (my_DATA_pck.block_num), (int)sizeof(my_DATA_pck));
			unsigned int lastWriteSize = fwrite((ntohs(my_DATA_pck.data)), sizeof(char), (unsigned int)received_bytes - 4,fp); // write next bulk of data
		   	printf("WRITING: %d\n", ((int)strlen(ntohs(my_DATA_pck.data))));
			
			// TODO: send ACK packet to the client
			ack_count++;
			my_ACK_pck.block_num = htons(ack_count);
			if (sendto(server_socketfd, (void*)&my_ACK_pck, sizeof(my_ACK_pck), 0, (struct sockaddr*) & client_addr, &client_addr_len) == -1)
			{
				perror("TTFTP_ERROR:Error in sendto \n");
				fclose(fp);

				clear_socket(server_socketfd);
				close(server_socketfd);
				exit(1);
			}
			
			printf("OUT:ACK,%d\n", ack_count);
		} while (received_bytes>HEADER_SIZE);// Have blocks left to be read from client (not end of transmission)
		fclose(fp);
		clear_socket(server_socketfd);
		if (end_transfer)
			printf("RECVFAIL\n");
		else
			printf("RECVOK\n");
	}

	 

	return 0;
}

//********************************************
// function name:clear_socket
// Description: clear the socket from R/W
// Parameters: file descriptor of socket
// Returns: nothing(void)
//*********************************************
void clear_socket(int socket)
{
	int size = 0;
	if (ioctl(socket, FIONREAD, &size) < 0)
	{
		printf("TTFTP_ERROR: Error in ioctl \n");
		close(socket);
		exit(1);
	}
	return;

}



