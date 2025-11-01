#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include "MessageType.h"

#define PORT 1234 
#define MAX_CLIENTS 10

int main() {
	int listener_fd, new_fd, i, rv;
	new_fd = -1;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addrlen;
	struct pollfd fds[MAX_CLIENTS + 1];

	// Create the listener socket
	if ((listener_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Prepare server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

	// Bind the socket
	if (bind(listener_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	// Listen for connections
	if (listen(listener_fd, 10) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);

	// Initialize the pollfd array
	fds[0].fd = listener_fd;
	fds[0].events = POLLIN;
	for (i = 1; i <= MAX_CLIENTS; i++) {
		fds[i].fd = -1; // -1 indicates an unused slot
	}

	while (1) {
		// Poll for events, with a 1-second timeout
		if ((rv = poll(fds, MAX_CLIENTS + 1, 1000)) == -1) {
			perror("poll");
			exit(EXIT_FAILURE);
		}

		if (rv == 0) {
			// No events, continue polling
			continue;
		}

		// Check for new client connections
		if (fds[0].revents & POLLIN) {
			addrlen = sizeof(client_addr);
			if ((new_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addrlen)) == -1) {
				perror("accept");
			} else {
				Message Announce;
				int n = read (new_fd, &Announce, sizeof(Message));
				if (!n) printf("first read failed, %d \n", new_fd);
				// Add the new connection to the pollfd array
				for (i = 1; i <= MAX_CLIENTS; i++) {
					if (fds[i].fd == -1) {
						fds[i].fd = new_fd;
						fds[i].events = POLLIN;
						printf("New client connected on socket %d\n", new_fd);
						break;
					}
					else {
						if (Announce.Type == Announcement) {
							send(new_fd, &Announce, sizeof(Announce), 0);
							printf("SENT!!!! \n");
						}
					}
				}
				if (i > MAX_CLIENTS) {
					fprintf(stderr, "Maximum clients reached, rejecting new connection.\n");
					close(new_fd);
				}
			}
		}

		//Now stream to client......

		FILE *fp;                                                                         
		fp = fopen("airplane-landing_daniel_simion.wav", "rb"); // Open your WAV file     
		if (fp == NULL) {
        		perror("Error opening file for writing");
        		return 1; // Indicate an error
    		}						// ... error handling ... 

		Message audMsg;                                                                   
		audMsg.Type = Audio;                                                              
		while (!feof(fp) && new_fd != -1) {                                                               
			size_t bytes_read = fread(audMsg.Data, 1, sizeof(audMsg.Data), fp);       
			if (bytes_read == sizeof(Message) ) {                                     
				send(new_fd, &audMsg, sizeof(Message), 0);                   
			}                                                                         
			else{                                                                     
				Message tmp = {0};                                                
				tmp.Type = Audio;                                                 
				memcpy(&tmp.Data, &audMsg.Data, bytes_read);                      
				send(new_fd, &tmp, sizeof(Message), 0);                      
			}                                                                         

		}       
	}

	close(listener_fd);
	return 0;
}

