// threadpool.h
// Created by Olajide Akinyemi on 11/26/25.
// Updated by PK for Sprint 1

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

// Define a task (a single client connection)
typedef struct task {
    int client_fd;
    struct task *next;
} task_t;

// Define the thread pool manager
typedef struct {
    pthread_mutex_t lock;       // Mutex to protect the queue
    pthread_cond_t notify;      // Condition variable to wake up workers
    pthread_t *threads;         // Array of threads
    task_t *queue_head;         // Front of the queue
    task_t *queue_tail;         // End of the queue
    int thread_count;           // Number of active threads
    int queue_size;             // Current tasks waiting
    int shutdown;               // Flag to signal shutdown
} threadpool_t;

// Initialize thread pool
void threadpool_init(int num_threads);

// Enqueue a client socket for workers to process
void enqueue_client(int client_file_descriptor);

// Cleanup (Optional but good practice)
void threadpool_destroy();

#endif