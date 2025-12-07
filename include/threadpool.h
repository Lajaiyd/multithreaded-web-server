//
// Created by Olajide Akinyemi on 11/26/25.
// Implemented by PK
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>

// ============================================
// Configuration Constants
// ============================================
#define DEFAULT_THREAD_COUNT 4
#define MAX_QUEUE_SIZE 256

// ============================================
// Request Queue Node
// ============================================
typedef struct queue_node {
    int client_fd;
    struct queue_node *next;
} queue_node_t;

// ============================================
// Synchronized Request Queue
// Uses mutex + condition variable for thread safety
// ============================================
typedef struct request_queue {
    queue_node_t *head;
    queue_node_t *tail;
    int size;
    int max_size;
    
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;   // Signal when queue has items
    pthread_cond_t not_full;    // Signal when queue has space
} request_queue_t;

// ============================================
// Thread Pool Structure
// ============================================
typedef struct threadpool {
    pthread_t *threads;         // Array of worker threads
    int thread_count;           // Number of threads in pool
    request_queue_t queue;      // Synchronized request queue
    bool shutdown;              // Flag for graceful shutdown
} threadpool_t;

// ============================================
// Public API
// ============================================

// Initialize thread pool with specified number of worker threads
// Returns 0 on success, -1 on failure
int threadpool_init(int num_threads);

// Enqueue a client socket for workers to process
// Blocks if queue is full
// Returns 0 on success, -1 on failure
int enqueue_client(int client_file_descriptor);

// Gracefully shutdown the thread pool
// Waits for all workers to finish current tasks
void threadpool_shutdown(void);

// Get current queue size (for monitoring/debugging)
int threadpool_queue_size(void);

#endif // THREADPOOL_H