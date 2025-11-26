// Implementation of thread pool functions (stub) to be completed by PK

#include "threadpool.h"
#include <stdio.h>

void threadpool_init(int num_threads) {
    printf("[ThreadPool] Initialized with %d threads (stub)\n", num_threads);
}

void enqueue_client(int client_file_descriptor) {
    printf("[ThreadPool] Enqueued client fd=%d (stub)\n", client_file_descriptor);
}
