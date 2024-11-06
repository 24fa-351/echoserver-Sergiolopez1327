#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 50001
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 8080

void *handleConnection(void *arg) {
    int clientSocket = *(int *)arg;
    free(arg);  // Free the dynamically allocated memory for the client socket
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';  // Null-terminate the received data
        printf("Received: %s", buffer);
        write(clientSocket, buffer, bytesRead);  // Echo back to the client
    }

    printf("Client disconnected.\n");
    close(clientSocket);
    return NULL;
}

int main(int argc, char *argv[]) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, LISTEN_BACKLOG) == -1) {
        perror("listen");
        close(serverSocket);
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int *clientSocket = malloc(sizeof(int));
        if (!clientSocket) {
            perror("malloc");
            continue;
        }

        *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (*clientSocket == -1) {
            perror("accept");
            free(clientSocket);
            continue;
        }

        printf("Connected to client\n");

        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, handleConnection, clientSocket) != 0) {
            perror("pthread_create");
            close(*clientSocket);
            free(clientSocket);
            continue;
        }

        // Detach the thread to handle its own cleanup
        pthread_detach(clientThread);
    }

    close(serverSocket);
    return 0;
}
