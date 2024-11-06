#ifndef ECHO_H
#define ECHO_H

#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 5

void *handle_connection(void *arg);

#endif