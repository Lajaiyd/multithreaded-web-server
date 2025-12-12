// concurrent_test.c - Concurrent stress test for thread pool
// Implemented by PK
//
// Tests the server with multiple simultaneous connections
// to verify thread pool handles concurrency correctly

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

#define SERVER_PORT 8081
#define SERVER_IP "127.0.0.1"
#define NUM_CLIENTS 20
#define NUM_REQUESTS_PER_CLIENT 5

typedef struct {
    int thread_id;
    int requests_sent;
    int requests_successful;
    int requests_failed;
    double total_time_ms;
} client_stats_t;

static client_stats_t stats[NUM_CLIENTS];
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

static double get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}

static void safe_print(const char *format, ...) {
    va_list args;
    pthread_mutex_lock(&print_mutex);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
    pthread_mutex_unlock(&print_mutex);
}

static int make_request(int client_id, int request_num) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024];
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        safe_print("[Client %d] Socket creation failed: %s\n", 
                   client_id, strerror(errno));
        return -1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        safe_print("[Client %d] Invalid address\n", client_id);
        close(sock);
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        safe_print("[Client %d] Connection failed: %s\n", 
                   client_id, strerror(errno));
        close(sock);
        return -1;
    }
    
    char request[256];
    snprintf(request, sizeof(request),
             "GET /test?client=%d&req=%d HTTP/1.0\r\n"
             "Host: localhost\r\n"
             "\r\n",
             client_id, request_num);
    
    if (send(sock, request, strlen(request), 0) < 0) {
        safe_print("[Client %d] Send failed: %s\n", 
                   client_id, strerror(errno));
        close(sock);
        return -1;
    }
    
    ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) {
        safe_print("[Client %d] Receive failed: %s\n", 
                   client_id, strerror(errno));
        close(sock);
        return -1;
    }
    
    buffer[bytes] = '\0';
    
    if (strstr(buffer, "200 OK") == NULL) {
        safe_print("[Client %d] Invalid response\n", client_id);
        close(sock);
        return -1;
    }
    
    close(sock);
    return 0;
}

static void *client_thread(void *arg) {
    int client_id = *(int *)arg;
    free(arg);
    
    client_stats_t *my_stats = &stats[client_id];
    my_stats->thread_id = client_id;
    my_stats->requests_sent = 0;
    my_stats->requests_successful = 0;
    my_stats->requests_failed = 0;
    my_stats->total_time_ms = 0;
    
    safe_print("[Client %d] Started\n", client_id);
    
    for (int i = 0; i < NUM_REQUESTS_PER_CLIENT; i++) {
        double start = get_time_ms();
        
        int result = make_request(client_id, i);
        my_stats->requests_sent++;
        
        double elapsed = get_time_ms() - start;
        my_stats->total_time_ms += elapsed;
        
        if (result == 0) {
            my_stats->requests_successful++;
            safe_print("[Client %d] Request %d succeeded (%.2f ms)\n", 
                       client_id, i, elapsed);
        } else {
            my_stats->requests_failed++;
            safe_print("[Client %d] Request %d failed\n", client_id, i);
        }
        
        usleep(10000);
    }
    
    safe_print("[Client %d] Finished\n", client_id);
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_CLIENTS];
    
    printf("  Concurrent Thread Pool Test\n");
    printf("Server: %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Clients: %d\n", NUM_CLIENTS);
    printf("Requests per client: %d\n", NUM_REQUESTS_PER_CLIENT);
    printf("Total requests: %d\n", NUM_CLIENTS * NUM_REQUESTS_PER_CLIENT);
    
    double start_time = get_time_ms();
    
    for (int i = 0; i < NUM_CLIENTS; i++) {
        int *client_id = malloc(sizeof(int));
        *client_id = i;
        
        if (pthread_create(&threads[i], NULL, client_thread, client_id) != 0) {
            perror("Failed to create client thread");
            free(client_id);
            return EXIT_FAILURE;
        }
    }
    
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double total_time = get_time_ms() - start_time;
    

    printf("  Test Results Summary\n");
    int total_sent = 0;
    int total_success = 0;
    int total_failed = 0;
    double total_latency = 0;
    
    for (int i = 0; i < NUM_CLIENTS; i++) {
        total_sent += stats[i].requests_sent;
        total_success += stats[i].requests_successful;
        total_failed += stats[i].requests_failed;
        total_latency += stats[i].total_time_ms;
    }
    
    printf("Total requests sent:      %d\n", total_sent);
    printf("Successful requests:      %d\n", total_success);
    printf("Failed requests:          %d\n", total_failed);
    printf("Success rate:             %.1f%%\n", 
           (total_success * 100.0) / total_sent);
    printf("Total test time:          %.2f ms\n", total_time);
    printf("Average latency:          %.2f ms\n", 
           total_latency / total_sent);
    printf("Throughput:               %.1f requests/sec\n",
           (total_sent * 1000.0) / total_time);
    
    return (total_failed > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}