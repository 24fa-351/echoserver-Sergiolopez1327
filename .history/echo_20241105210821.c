

#define PORT 50001
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 8080

void handleConnection(int clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    printf("Recieved: %s\n", buffer);
    write(clientSocket, buffer, bytesRead);
}