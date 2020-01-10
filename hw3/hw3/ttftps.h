#ifndef TTFTP_H
#define TTFTP_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>//TODO check if its simple to use for writing
#include <netinet/in.h>
#define DATA_LEN 512
#define MAX_LEN DATA_LEN*2
#define WRQ_OPCODE 2
#define ACK_OPCODE 4
#define DATA_OPCODE 3
#define WAIT_FOR_PACKET_TIMEOUT 3
#define NUMBER_OF_FAILURES 7
#define PKT_FIFO_LEN 5

struct WRQ_pkt {
	unsigned short opcode;
	string file_name;
	//char string_terminator = 0;
	string trans_mode;
	//char string_terminator2 = 0;
} __attribute__((packed));

struct ACK_pck {
	unsigned short opcode;
	unsigned short blockNum;
} __attribute__((packed));

struct DATA_pck {
	unsigned short opcode;
	unsigned short blockNum;
	char data[MAX_BYTES];
} __attribute__((packed));


#endif //TTFTP_H