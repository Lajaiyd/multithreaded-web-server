//
// Created by Olajide Akinyemi on 11/26/25.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

// Initialize thread pool
void threadpool_init(int num_threads);

// Enqueue a client socket for workers to process
void enqueue_client(int client_file_descriptor);

#endif