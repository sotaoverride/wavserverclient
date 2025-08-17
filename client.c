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
	char recvBuff[1024]={'\0'};
	struct sockaddr_in serv_addr;
	int server_port = atoi(argv[2]); // Convert port string to integer
	char * client_name = argv[3];
	if(argc != 4)
	{
		fprintf(stderr, "\n Usage: %s <ip of server> <port of server> <client name>\n",argv[0]);
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
	sprintf(goMsgStr, "Hello from:  %s!", client_name);  
	size_t length = strlen(goMsgStr);
	printf("lendgth of gstr in client %ld", length);
	goMsgStr[length+1]='\0';
	n = send(sockfd, goMsgStr, length+1, 0);
	if(n < length)
	{
		fprintf(stderr, "\n Send error?? \n");
	}
	int wavBytesRead = 0;
	while ( (n = read(sockfd, recvBuff, sizeof(recvBuff))) > 0)
	{
		if (!(strstr(recvBuff, "RIFF"))){
			       //start of audio stream, so quit printing announcments for ~ 	2494872/1024 reads (size of wav file)
			       wavBytesRead=0;
			       }
		if (wavBytesRead < 249872){
		if (fwrite(recvBuff, 1, n , stdout) !=n ) {
			fprintf(stderr, "\n Error: Fwrite errir\n");
		}
		wavBytesRead+=1024;}
		else{
		printf("LENGTH OF STRING READ BY CLIENT: %d \n", n);
		recvBuff[n+1]='\0';
		printf("%s \n", recvBuff);
		}
	}

	if(n < 0)
	{
		fprintf(stderr, "\n Read error \n");
	}

	return 0;
}
