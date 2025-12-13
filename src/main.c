#include "server.h"
#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static int g_server_fd = -1;

void signal_handler(int sig) {
    printf("\nReceived signal %d, shutting down...\n", sig);
    
    threadpool_shutdown();
    
    if (g_server_fd >= 0) {
        close(g_server_fd);
    }
    
    exit(0);
}

int main(void) {
    int port = 8081;  
    int num_threads = 4;  
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("Initializing thread pool with %d workers...\n", num_threads);
    if (threadpool_init(num_threads) != 0) {
        fprintf(stderr, "Failed to initialize thread pool\n");
        return EXIT_FAILURE;
    }
    
    int server_file_descriptor = start_server(port);
    g_server_fd = server_file_descriptor;  
    main_accept_loop(server_file_descriptor);
    threadpool_shutdown();
    close(server_file_descriptor);
    
    return 0;
}