//
// Created by Olajide Akinyemi on 11/24/25.
// Updated by PK for Sprint 1 integration
//
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "handler.h"
#include "threadpool.h"

#define SERVER_BACKLOG 10
#define BUFFER_SIZE 1024

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

int start_server(int server_port) {
    int server_file_descriptor;
    SA_IN server_addr;

    // Create socket
    if ((server_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Allow port reuse
    int opt = 1;
    setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Initialize address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    // Bind socket to address
    if (bind(server_file_descriptor, (SA*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket bind failed");
        close(server_file_descriptor);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_file_descriptor, SERVER_BACKLOG) < 0) {
        perror("Failed to start listening");
        close(server_file_descriptor);
        exit(EXIT_FAILURE);
    }

    printf("Server started and listening on port %d\n", server_port);
    return server_file_descriptor;
}

int accept_connection(int server_file_descriptor)
{
    SA_IN client;
    socklen_t len = sizeof(client);

    int client_file_descriptor =
        accept(server_file_descriptor, (SA*)&client, &len);

    if (client_file_descriptor < 0) {
        perror("Connection accept failed");
        return -1;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
    printf("New client connected from %s:%d\n",
           ip, ntohs(client.sin_port));

    return client_file_descriptor;
}

int main_accept_loop(int server_file_descriptor) {
    printf("Starting server main loop (Sprint 1 - Thread Pool)...\n");

    while (1) {
        int client_file_descriptor = accept_connection(server_file_descriptor);
        if (client_file_descriptor < 0) {
            continue;  
        }

        if (enqueue_client(client_file_descriptor) != 0) {
            fprintf(stderr, "Failed to enqueue client, closing connection\n");
            close(client_file_descriptor);
        }
    }
}