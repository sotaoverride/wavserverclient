#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "linkedlist.h"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
typedef struct {
	int sock;
	struct Node* head;
} ThreadArgs;
FILE *wav_file;
char *wav_file_path = "airplane-landing_daniel_simion.wav";

pthread_mutex_t the_mutex; // Declare a mutex
void *handle_client(void *args) {
	char goMsgRecv[40]={'\0'};
	ThreadArgs* argsPtr = (ThreadArgs*)args;
	int client_sock = argsPtr->sock;
	//first read
	int n = read(client_sock, goMsgRecv, 40);
	if (!n) printf("frist read failed,  %d \n", client_sock);
	pthread_mutex_lock(&the_mutex); // Acquire lock
	struct Node* tmp = argsPtr->head;
	pthread_mutex_unlock(&the_mutex); // Release lock
	pthread_mutex_lock(&the_mutex); // Acquire lock
	while (tmp != NULL) {
		if(tmp->data != client_sock){
			send(tmp->data, "Hello from new cleint", strlen("Hello from new client"), 0);
			
		}
		
		tmp = tmp->next;
	}
	pthread_mutex_unlock(&the_mutex); // Release lock
	while(1){
			n = read(client_sock, goMsgRecv, 40);
		}
	close(client_sock);
	pthread_exit(NULL);
}

int main() {
	struct Node* head = NULL; 

	int server_sock, client_sock;
	struct sockaddr_in address, client_addr;
	socklen_t client_len;

	// Create socket
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	// ... bind and listen ...
	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));


	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(SERVER_PORT);

	// Bind socket to port
	if (bind(server_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for connections
	if (listen(server_sock, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while (1) {
		client_len = sizeof(client_addr);
		client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
		if (client_sock < 0) {
			perror("accept failed");
		}
		else { //insert client socket in linkedlist!
			pthread_mutex_lock(&the_mutex); // Acquire lock
			insertAtBeginning(&head, client_sock);
			pthread_mutex_unlock(&the_mutex); // Release lock
		}

		pthread_t thread_id;
		ThreadArgs *args = malloc(sizeof(ThreadArgs)); // Allocate dynamically for persistence
		if (args == NULL) {
			// Handle allocation error
		}
		args->sock = client_sock;
		args->head = head;

		if (pthread_create(&thread_id, NULL, handle_client, (void*)args) < 0) {
			perror("could not create thread");
			close(client_sock);
			free(head);
			continue;
		}
		pthread_detach(thread_id); // Detach the thread to clean up resources automatically
	}

	close(server_sock);
	return 0;
}
