// threadpool.c - Thread Pool Implementation
// Implemented by PK

#include "threadpool.h"
#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static threadpool_t pool;
static bool pool_initialized = false;

static int queue_init(request_queue_t *q, int max_size) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    q->max_size = max_size;
    
    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        perror("[ThreadPool] Failed to initialize mutex");
        return -1;
    }
    if (pthread_cond_init(&q->not_empty, NULL) != 0) {
        perror("[ThreadPool] Failed to initialize not_empty cond var");
        pthread_mutex_destroy(&q->mutex);
        return -1;
    }
    if (pthread_cond_init(&q->not_full, NULL) != 0) {
        perror("[ThreadPool] Failed to initialize not_full cond var");
        pthread_mutex_destroy(&q->mutex);
        pthread_cond_destroy(&q->not_empty);
        return -1;
    }
    return 0;
}

static void queue_destroy(request_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    queue_node_t *current = q->head;
    while (current != NULL) {
        queue_node_t *next = current->next;
        close(current->client_fd);
        free(current);
        current = next;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}

static int queue_push(request_queue_t *q, int client_fd) {
    queue_node_t *node = malloc(sizeof(queue_node_t));
    if (node == NULL) {
        perror("[ThreadPool] Failed to allocate queue node");
        return -1;
    }
    node->client_fd = client_fd;
    node->next = NULL;
    pthread_mutex_lock(&q->mutex);
    while (q->size >= q->max_size && !pool.shutdown) {
        printf("[ThreadPool] Queue full (%d/%d), waiting...\n", 
               q->size, q->max_size);
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    if (pool.shutdown) {
        pthread_mutex_unlock(&q->mutex);
        free(node);
        return -1;
    }
    if (q->tail == NULL) {
        q->head = node;
        q->tail = node;
    } else {
        q->tail->next = node;
        q->tail = node;
    }
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

static int queue_pop(request_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0 && !pool.shutdown) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    if (pool.shutdown && q->size == 0) {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }
    queue_node_t *node = q->head;
    int client_fd = node->client_fd;
    q->head = node->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    q->size--;
    free(node);
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return client_fd;
}

static void *worker_routine(void *arg) {
    int thread_id = *(int *)arg;
    free(arg); 
    printf("[Worker %d] Started\n", thread_id);
    while (1) {
        int client_fd = queue_pop(&pool.queue);
        if (client_fd < 0) {
           
            printf("[Worker %d] Shutting down\n", thread_id);
            break;
        }
        printf("[Worker %d] Processing client fd=%d\n", thread_id, client_fd);
        handle_connection_stub(client_fd);
        printf("[Worker %d] Finished processing client fd=%d\n", 
               thread_id, client_fd);
    }
    return NULL;
}


int threadpool_init(int num_threads) {
    if (pool_initialized) {
        fprintf(stderr, "[ThreadPool] Already initialized\n");
        return -1;
    }
    if (num_threads <= 0) {
        num_threads = DEFAULT_THREAD_COUNT;
    }
    printf("[ThreadPool] Initializing with %d worker threads\n", num_threads);
    pool.thread_count = num_threads;
    pool.shutdown = false;
    if (queue_init(&pool.queue, MAX_QUEUE_SIZE) != 0) {
        return -1;
    }
    pool.threads = malloc(sizeof(pthread_t) * num_threads);
    if (pool.threads == NULL) {
        perror("[ThreadPool] Failed to allocate thread array");
        queue_destroy(&pool.queue);
        return -1;
    }
    for (int i = 0; i < num_threads; i++) {
        int *thread_id = malloc(sizeof(int));
        if (thread_id == NULL) {
            perror("[ThreadPool] Failed to allocate thread ID");
            pool.shutdown = true;
            pthread_cond_broadcast(&pool.queue.not_empty);
            for (int j = 0; j < i; j++) {
                pthread_join(pool.threads[j], NULL);
            }
            free(pool.threads);
            queue_destroy(&pool.queue);
            return -1;
        }
        *thread_id = i;
        if (pthread_create(&pool.threads[i], NULL, worker_routine, thread_id) != 0) {
            perror("[ThreadPool] Failed to create worker thread");
            free(thread_id);
            pool.shutdown = true;
            pthread_cond_broadcast(&pool.queue.not_empty);
            for (int j = 0; j < i; j++) {
                pthread_join(pool.threads[j], NULL);
            }
            free(pool.threads);
            queue_destroy(&pool.queue);
            return -1;
        }
    }
    pool_initialized = true;
    printf("[ThreadPool] Successfully initialized with %d workers\n", num_threads);
    return 0;
}

int enqueue_client(int client_file_descriptor) {
    if (!pool_initialized) {
        fprintf(stderr, "[ThreadPool] Not initialized\n");
        return -1;
    }
    if (pool.shutdown) {
        fprintf(stderr, "[ThreadPool] Shutting down, rejecting new clients\n");
        return -1;
    }
    printf("[ThreadPool] Enqueuing client fd=%d\n", client_file_descriptor);
    return queue_push(&pool.queue, client_file_descriptor);
}

void threadpool_shutdown(void) {
    if (!pool_initialized) {
        return;
    }
    printf("[ThreadPool] Initiating shutdown...\n");
    pthread_mutex_lock(&pool.queue.mutex);
    pool.shutdown = true;
    pthread_mutex_unlock(&pool.queue.mutex);
    pthread_cond_broadcast(&pool.queue.not_empty);
    pthread_cond_broadcast(&pool.queue.not_full);
    for (int i = 0; i < pool.thread_count; i++) {
        pthread_join(pool.threads[i], NULL);
        printf("[ThreadPool] Worker %d joined\n", i);
    }
    free(pool.threads);
    queue_destroy(&pool.queue);
    pool_initialized = false;
    printf("[ThreadPool] Shutdown complete\n");
}

int threadpool_queue_size(void) {
    if (!pool_initialized) {
        return -1;
    }
    pthread_mutex_lock(&pool.queue.mutex);
    int size = pool.queue.size;
    pthread_mutex_unlock(&pool.queue.mutex);
    return size;
}