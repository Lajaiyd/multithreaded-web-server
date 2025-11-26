// handler.c — HTTP request handler module
// Jide: temporary Sprint 0 stub
// Osa: full HTTP logic in Sprint 2
//

#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

void handle_connection_stub(int client_file_descriptor) {
    char buffer[1024];

    ssize_t bytes = recv(client_file_descriptor, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) {
        perror("Failed to receive data from client");
        close(client_file_descriptor);
        return;
    }

    buffer[bytes] = '\0';
    printf("Received %zd bytes from client:\n%s\n", bytes, buffer);

    // Sprint 0 - basic test response
    const char *msg =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello from Sprint 0 handler!\n";

    send(client_file_descriptor, msg, strlen(msg), 0);

    printf("Response sent to client, closing connection\n");
    close(client_file_descriptor);
}