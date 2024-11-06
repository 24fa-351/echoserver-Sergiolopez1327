#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 50001
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 8080

void handleConnection(int clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    printf("Recieved: %s\n", buffer);
    write(clientSocket, buffer, bytesRead);
}

int main(int argc, char* argv[]) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind");
        return 1;
    }

    if (listen(serverSocket, LISTEN_BACKLOG) == -1) {
        perror("listen");
        return 1;
    }

    while (1) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            perror("accept");
            return 1;
        }
        handleConnection(clientSocket);
        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}