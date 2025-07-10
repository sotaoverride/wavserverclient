#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "linkedlist.h"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() {
	//socket linked list head
	struct Node* head = NULL;
	// Child process id
	pid_t childpid;
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	size_t bytes_read;

	// Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));


	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(SERVER_PORT);

	// Bind socket to port
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for connections
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while (1) {
		// Accept connection
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		else { //insert client socket in linkedlist!
			insertAtBeginning(&head, new_socket);
		}


		if ((childpid = fork() == 0)){
			//remove yourself from linked list
			deleteFromBeginning(&head);
			while(1){
				if ( (bytes_read = read(new_socket, buffer, BUFFER_SIZE)) >0 ){
				struct Node* tmp = head;
				while (tmp != NULL) {
					send(new_socket, buffer, bytes_read, 0);
					tmp = tmp->next;
				}
			}
			}
		}
		//clean up parent version of new sock
		close(new_socket);
	}
	//listening sock close
	close(server_fd);

	return 0;
}
