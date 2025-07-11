#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include <pthread.h>
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
typedef struct {
	int sock;
	struct Node* head;
} ThreadArgs;

pthread_mutex_t the_mutex; // Declare a mutex
void *handle_client(void *args) {
	ThreadArgs* argsPtr = (ThreadArgs*)args;
	int client_sock = argsPtr->sock;
	//lets free args->sock??
	free(&argsPtr->sock);	
	//shared struct so proct rads with mutex??
	struct Node* tmp = argsPtr->head;
	// Implement client communication here (send/recv)
	printf("Handling client socket: %d\n", client_sock);
	char buffer[1024];
	ssize_t bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0) {
		printf("Received from client %d: %s\n", client_sock, buffer);
		while (tmp) {
			if(argsPtr->sock != tmp->data){
				send(tmp->data, buffer, bytes_received, 0);
			}
			pthread_mutex_lock(&the_mutex); // Acquire lock
			tmp = tmp->next;

		}
		pthread_mutex_unlock(&the_mutex); // Release lock
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
			continue;
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
