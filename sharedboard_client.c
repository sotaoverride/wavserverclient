#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "MessageType.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
pthread_mutex_t printf_mutex; // Declare a mutex
pthread_mutex_t sock_mutex; // Declare a mutex
typedef struct {
	int recvdMsgCount;
	// other members
} ClientStats;

// Static pointer to hold the single instance
static ClientStats* global_singleton_client_stats_instance = NULL;

// Function to initialize and get the singleton instance
ClientStats* get_singleton_client_stats_instance() {
	if (global_singleton_client_stats_instance == NULL) {
		// Allocate memory for the singleton
		global_singleton_client_stats_instance = (ClientStats*)malloc(sizeof(ClientStats));
		if (global_singleton_client_stats_instance == NULL) {
			perror("Failed to allocate memory for client stats");
			exit(EXIT_FAILURE);
		}
		// Initialize members
		global_singleton_client_stats_instance->recvdMsgCount = 0; 
		printf("Client stats Singleton instance created.\n");
	}
	return global_singleton_client_stats_instance;
}

// Function to clean up the singleton (optional, but good practice)
void destroy_singleton_client_stats_instance() {
	if (global_singleton_client_stats_instance != NULL) {
		free(global_singleton_client_stats_instance);
		global_singleton_client_stats_instance = NULL;
		printf("Singleton client stats instance destroyed.\n");
	}
}
ClientStats* CS = NULL;
typedef struct ArgSRT{
	int sockFD;
	ClientStats cs;
} ASRT;
void *input_thread_func(void *arg) {
	char buffer[256];
	fd_set read_fds;
	struct timeval tv;
	int retval;
	int sockfd = *(int *)(arg);


	// Set a timeout for select (e.g., 1 second)
	tv.tv_sec = 8;
	tv.tv_usec = 0;
	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		// Set a timeout for select (e.g., 1 second)
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		retval = select(STDIN_FILENO +1, &read_fds, NULL, NULL, &tv);

		if (retval == -1) {
			perror("select");
			printf("broken out of while?? \n");
			break;
		} else if (retval) {
			// STDIN is ready to be read
			ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
			if (bytes_read > 0 && (buffer[bytes_read -1] == '\n')) {
				buffer[bytes_read] = '\0';
				printf("Input received: %s", buffer);
				send(sockfd, &buffer, strlen(buffer), 0);
			} else if (bytes_read == 0) {
				printf("End of input (EOF)\n");
				break;
			} else {
				perror("read");
				break;
			}
		} else {
			// Timeout occurred, no input
			printf("No input for 1 second...\n");
		}
	}
	return NULL;
}

void *sock_read_thread_func(void *arg) {
	//Message recvMsg;
	//memset(recvMsg.Data, '\0', 256);
	char recvbuf[256] = {'\0'};
	ASRT * argPtr= (ASRT *) arg;
	int m =0;
	while(1){
		m = read(argPtr->sockFD, recvbuf, 256);

		if (m>0){
			//print message data
			argPtr->cs.recvdMsgCount++;
			printf("%s \n", recvbuf);
			printf("received messag count for this client %d \n" , argPtr->cs.recvdMsgCount); 
		}
		else{
			fprintf(stderr, "\n Read earror read return value %d \n", m);
		}
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	CS = get_singleton_client_stats_instance();
	pthread_t input_thread, sock_read_thread;
	int sockfd = 0;
	struct sockaddr_in serv_addr;
	int server_port = atoi(argv[2]); // Convert port string to integer
	if(argc != 4)
	{
		fprintf(stderr, "\n Usage: %s <ip of server> <port of server> <client name>\n",argv[0]);
		return 1;
	}


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


	// Set STDIN to non-blocking mode
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	// Create the input handling thread
	if (pthread_create(&input_thread, NULL, input_thread_func, (void *) &sockfd) != 0) {
		perror("pthread_create");
		return 1;
	}
	ASRT asrt;
	asrt.sockFD = sockfd;
	asrt.cs = *CS;
	// Create the sock read handling thread
	if (pthread_create(&sock_read_thread, NULL, sock_read_thread_func, (void *) &asrt) != 0) {
		perror("pthread_create");
		return 1;
	}


	// Join the input thread (optional, depending on application logic)
	pthread_join(input_thread, NULL);

	// Join the sock read  thread (optional, depending on application logic)
	pthread_join(sock_read_thread, NULL);
	// Restore STDIN to blocking mode (optional)
	fcntl(STDIN_FILENO, F_SETFL, flags);

	return 0;
}
