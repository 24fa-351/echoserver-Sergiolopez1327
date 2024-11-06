#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "echo.h"


void *handle_connection(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);  
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("Client connected.\n");

    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';  
        printf("Received: %s", buffer);

        if (send(client_socket, buffer, bytes_read, 0) == -1) {
            perror("send");
            break;
        }
    }

    printf("Client disconnected.\n");
    close(client_socket);
    return NULL;
}
