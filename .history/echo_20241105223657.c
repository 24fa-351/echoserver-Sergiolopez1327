#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <ctype.h>

#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 1024

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

int main(int argc, char *argv[]) {
    int port = 50001;  // Default port
    int option;

    // Parse command-line arguments for port (-p option)
    while ((option = getopt(argc, argv, "p:")) != -1) {
        switch (option) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Create server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the specified port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_socket, LISTEN_BACKLOG) == -1) {
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

        // Detach the thread to allow automatic resource cleanup
        pthread_detach(client_thread);
    }

    close(server_socket);
    return 0;
}
