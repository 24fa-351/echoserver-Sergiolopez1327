#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "echo.h"

int main(int argc, char *argv[]) {
    int server_socket;
    struct sockaddr_in server_address;
    int port = 8080;

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        int *client_socket = malloc(sizeof(int));
        if (!client_socket) {
            perror("malloc");
            continue;
        }

        // Accept a new client connection
        *client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (*client_socket == -1) {
            perror("accept");
            free(client_socket);
            continue;
        }

        // Create a new thread for the client connection
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_connection, client_socket) != 0) {
            perror("pthread_create");
            close(*client_socket);
            free(client_socket);
            continue;
        }

        // Detach the thread so that it cleans up after itself
        pthread_detach(client_thread);
    }

    close(server_socket);
    return 0;
}