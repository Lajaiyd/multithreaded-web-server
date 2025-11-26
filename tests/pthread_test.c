//
// Created by Olajide Akinyemi on 11/25/25.
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* worker(void* arg) {
    printf("[Worker] Hello from thread! ID: %p\n",(void*) pthread_self());
    sleep(1);
    return NULL;
}

int main(void) {
    pthread_t thread;
    printf("[Main] Creating test thread...\n");

    if (pthread_create(&thread, NULL, worker, NULL) != 0) {
        perror("[Error] pthread_create failed");
        return EXIT_FAILURE;
    }

    if (pthread_join(thread, NULL) != 0) {
        perror("[Error] pthread_join failed");
        return EXIT_FAILURE;
    }

    printf("[Main] Thread joined successfully! Environment OK\n");
    return EXIT_SUCCESS;
}
