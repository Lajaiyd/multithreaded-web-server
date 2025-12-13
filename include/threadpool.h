//
// Created by Olajide Akinyemi on 11/26/25.
// Updated by PK - Sprint 1
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>

#define DEFAULT_THREAD_COUNT 4
#define MAX_QUEUE_SIZE 256

typedef struct queue_node {
    int client_fd;
    struct queue_node *next;
} queue_node_t;

typedef struct request_queue {
    queue_node_t *head;
    queue_node_t *tail;
    int size;
    int max_size;
    
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;   
    pthread_cond_t not_full;    
} request_queue_t;

typedef struct threadpool {
    pthread_t *threads;         
    int thread_count;           
    request_queue_t queue;      
    bool shutdown;              
} threadpool_t;


int threadpool_init(int num_threads);
int enqueue_client(int client_file_descriptor);
void threadpool_shutdown(void);
int threadpool_queue_size(void);

#endif // THREADPOOL_H