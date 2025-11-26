//
// Created by Olajide Akinyemi on 11/23/25.
//
//
// pthread_stress_test.c — multi-threaded stress test
// Verifies system concurrency before Sprint 1
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_COUNT 10

void* worker(void* arg) {
    int id = *(int*)arg;
    printf("[Worker %d] Started on thread ID: %p\n", id, (void*)pthread_self());
    usleep(200000); // 0.2 sec simulated work
    printf("[Worker %d] Finished!\n", id);
    return NULL;
}

int main(void) {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    printf("[Main] Launching %d worker threads...\n", THREAD_COUNT);

    for (int i = 0; i < THREAD_COUNT; i++) {
        ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, worker, &ids[i]) != 0) {
            perror("[Error] pthread_create failed");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[Main] All threads joined successfully! System concurrency OK ✅\n");
    return EXIT_SUCCESS;
}
