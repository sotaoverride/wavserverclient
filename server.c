#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    FILE *wav_file;
    char *wav_file_path = "airplane-landing_daniel_simion.wav";
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

    // Accept connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Open WAV file
    wav_file = fopen(wav_file_path, "rb");
    if (wav_file == NULL) {
        perror("Error opening WAV file");
        close(new_socket);
        exit(EXIT_FAILURE);
    }

    // Send WAV header (first 44 bytes)
    size_t val = fread(buffer, 1, 44, wav_file);
    if (val == 0) {
        perror("Error reading wav  file");
        exit(EXIT_FAILURE);
    }
    send(new_socket, buffer, 44, 0);


    // Stream audio data
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, wav_file)) > 0) {
        send(new_socket, buffer, bytes_read, 0);
    }

    printf("Finished streaming.\n");

    // Cleanup
    fclose(wav_file);
    close(new_socket);
    close(server_fd);

    return 0;
}
