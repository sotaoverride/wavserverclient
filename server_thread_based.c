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
	ThreadArgs* argsPtr = (ThreadArgs*)args;
	int client_sock = argsPtr->sock;
	//send this sock Go [sock no here] message
	char goMsg[20];

	sprintf(goMsg, "Go Sock %d!", client_sock); 
	size_t goMsgLen = strlen(goMsg);
	send(client_sock, goMsg, goMsgLen, 0);
	
	//lets free args->sock??
	//shared struct so proctx reads with mutex??
	pthread_mutex_lock(&the_mutex); // Acquire lock
	struct Node* tmp = argsPtr->head;
	pthread_mutex_unlock(&the_mutex); // Release lock
					  // Implement client communication here (send/recv)
	printf("Handling client socket: %d\n", client_sock);
	char buffer[1024];
	ssize_t bytes_read;	
	wav_file = fopen(wav_file_path, "rb");
	int count = 0;
	pthread_mutex_lock(&the_mutex); // Acquire lock
	printf("here : \n");
	while (tmp != NULL){
		printf("linked list read here :%d \n", count);
		count++;
		tmp = tmp->next;
	}
	printf("sock count : %d\n", count);

	// Step 2: Dynamically allocate the array
	int* arr = (int*)malloc(count * sizeof(int));
	if (arr == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
	}

	// Step 3: Populate the array
	tmp = argsPtr->head;
	struct Node* current = tmp;
	int i = 0;
	while (current != NULL) {
		arr[i] = current->data;
		printf("Sock from linked list : %d\n", arr[i]);
		current = current->next;
		i++;
	}
	pthread_mutex_unlock(&the_mutex); // Release lock
	while(1){
		for(int j=0;j<count;j++){
			while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, wav_file)) > 0) {
				//printf("Read from wav : %s\n", buffer);
				send(arr[j], buffer, bytes_read, 0);
			}
		}
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
