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

int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0;
	char recvBuff[1024];
	struct sockaddr_in serv_addr;
	int server_port = atoi(argv[2]); // Convert port string to integer
	if(argc != 3)
	{
		fprintf(stderr, "\n Usage: %s <ip of server> <port of server>\n",argv[0]);
		return 1;
	}

	memset(recvBuff, '0',sizeof(recvBuff));

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
	sprintf(goMsgStr, "Go Sock %d!", sockfd);  
	size_t length = strlen(goMsgStr);
	printf("lendgth of gstr in client %ld", length);
	goMsgStr[length+1]='\0';
	char recvGo[length+1];
	//wait on go sock message from server
	n = send(sockfd, goMsgStr, 40, 0);
	while ( (n = read(sockfd, recvGo, length)) > 0)
	{
		printf("recv msg: %s \n", recvGo);
		if (!strcmp(goMsgStr,recvGo)){
			printf("Go sock string found and Matched \n");
			break;
		} 
		else{
			printf("Go sock string recvd from server: %s \n", recvGo );
			n = send(sockfd, goMsgStr, 40, 0);
			}
		printf("still waiting on msg from server ... \n");
	}

	if(n < 0)
	{
		fprintf(stderr, "\n Read error \n");
	}
	while ( (n = read(sockfd, recvBuff, sizeof(recvBuff))) > 0)
	{
	}

	if(n < 0)
	{
		fprintf(stderr, "\n Read error \n");
	}

	return 0;
}
