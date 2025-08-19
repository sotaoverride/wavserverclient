/* --- client.c --- */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "MessageType.h"

int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0;
	Message recvMsg;
	struct sockaddr_in serv_addr;
	int server_port = atoi(argv[2]); // Convert port string to integer
	char * client_name = argv[3];
	if(argc != 4)
	{
		fprintf(stderr, "\n Usage: %s <ip of server> <port of server> <client name>\n",argv[0]);
		return 1;
	}

	memset(&recvMsg, '0',sizeof(recvMsg));

	/* a socket is created through call to socket() function */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "\n Error : Could not create socket \n");
		return 1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port);

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		fprintf(stderr, "\n inet_pton error occured\n");
		return 1;
	}

	/* Information like IP address of the remote host and its port is
	 * bundled up in a structure and a call to function connect() is made
	 * which tries to connect this socket with the socket (IP address and port)
	 * of the remote host
	 */
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "\n Error : Connect Failed \n");
		return 1;
	}

	/* Once the sockets are connected, the server sends the data (wav file data...)
	 * on clients socket through clients socket descriptor and client can read it
	 * through normal read call on the its socket descriptor.
	 */
	char goMsgStr[40]="\0";
	sprintf(goMsgStr, "Hello from:  %s!", client_name);  
	size_t length = strlen(goMsgStr);
	//printf("lendgth of gstr in client %ld", length);
	goMsgStr[length+1]='\0';
	recvMsg.Type = Announcement;
	memcpy(&recvMsg.Data, goMsgStr, length+1);
	n = send(sockfd, &recvMsg, sizeof(recvMsg), 0);
	if(n < length)
	{
		fprintf(stderr, "\n Send error?? \n");
	}


	FILE *sp;
	sp = fopen("stream.wav", "wb"); // Open your WAV file
	FILE *ap;
	ap = fopen("announcments.txt", "a+b"); // Open your WAV file
	setbuf(ap, NULL);
	while (1) {
		while ( (n = read(sockfd, &recvMsg, sizeof(recvMsg))) > 0  )
		{
				fprintf(stderr, "\n Read error \n");
			if ( n == sizeof(recvMsg)){
				if(recvMsg.Type == Audio){
					if (fwrite(recvMsg.Data, 1, n-sizeof(recvMsg.Type) , sp) !=n-sizeof(recvMsg.Type) ) {
						fprintf(stderr, "\n Error: Fwrite errir\n");
					}
				}
				else if(recvMsg.Type == Announcement) { if (fwrite(recvMsg.Data, 1, n-sizeof(recvMsg.Type) , ap) !=n-sizeof(recvMsg.Type) ) {
					fprintf(stderr, "\n Error: Fwrite errir\n");
				}
				}	
				else{
					printf(" Message Type not found !!!!!!!!!!!!\n");
				}
			}
			else {
				int j = n;
				int k = sizeof(recvMsg);
				while (j<k){
					char tmp2[k-j];//={0};
					int l  = read(sockfd, &tmp2, k-j);
					memcpy((char*)&recvMsg+j, tmp2, l);
					j = j + l;

				}

				if(recvMsg.Type == Audio){
					if (fwrite(recvMsg.Data, 1, n-sizeof(recvMsg.Type) , sp) !=n-sizeof(recvMsg.Type) ) {
						fprintf(stderr, "\n Error: Fwrite errir\n");
					}
				}
				else if(recvMsg.Type == Announcement) { if (fwrite(recvMsg.Data, 1, n-sizeof(recvMsg.Type) , ap) !=n-sizeof(recvMsg.Type) ) {
					fprintf(stderr, "\n Error: Fwrite errir\n");
				}
				}	
				else{
					printf(" Message Type not found !!!!!!!!!!!!\n");
				}
			}

		}
		
		fprintf(stderr, "\n Read error \n");
		exit(1);
	}


	return 0;
}
