#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "echo.h"

void *handle_connection(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);  // Free dynamically allocated memory for client socket
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("Client connected.\n");

    // Continuously read from client and echo back each line
    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the received data
        printf("Received: %s", buffer);

        // Echo back the received message to the client
        if (send(client_socket, buffer, bytes_read, 0) == -1) {
            perror("send");
            break;
        }
    }

    printf("Client disconnected.\n");
    close(client_socket);
    return NULL;
}
