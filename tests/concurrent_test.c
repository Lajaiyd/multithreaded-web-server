#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 8081
#define NUM_THREADS 20
#define REQUEST     "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"

void *client_thread(void *arg) {
    int id = *(int *)arg;
    free(arg);

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[4096];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        pthread_exit(NULL);
    }

    // Send HTTP request
    send(sockfd, REQUEST, strlen(REQUEST), 0);

    // Read response (we don’t care about full body here)
    ssize_t bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("[Client %d] Received response (%zd bytes)\n", id, bytes);
    }

    close(sockfd);
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    printf("Starting concurrent test with %d clients...\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        int *id = malloc(sizeof(int));
        *id = i;

        if (pthread_create(&threads[i], NULL, client_thread, id) != 0) {
            perror("pthread_create");
            free(id);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Concurrent test complete.\n");
    return 0;
}