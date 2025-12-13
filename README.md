A C-based HTTP/1.0 server capable of handling multiple client requests concurrently using POSIX threads.


# Thread Pool Implementation 

**Implemented by: Pratik Kamble**

## Overview

This is a complete thread pool implementation for the multithreaded web server with:
- Synchronized request queue using mutex + condition variables
- Configurable number of worker threads
- Graceful shutdown support
- Bounded queue to prevent memory exhaustion 

## Files Modified/Created

### `include/threadpool.h`
- `queue_node_t` - Linked list node for request queue
- `request_queue_t` - Synchronized queue with mutex/condvars
- `threadpool_t` - Thread pool structure

### `src/threadpool.c`
- `queue_init()` / `queue_destroy()` - Queue lifecycle
- `queue_push()` - Blocks if full, signals not_empty
- `queue_pop()` - Blocks if empty, signals not_full
- `worker_routine()` - Worker thread function
- `threadpool_init()` - Creates threads and queue
- `enqueue_client()` - Public API for main thread
- `threadpool_shutdown()` - Graceful shutdown

### `src/main.c`
- Added thread pool initialization
- Added signal handlers for graceful shutdown

### `src/server.c`
- `main_accept_loop()` now calls `enqueue_client()` instead of direct handling

### `tests/concurrent_test.c`
- Stress test with 20 clients × 5 requests each

## Building & Running

```bash
make clean && make

make run

# In another terminal, build and run the concurrent test
gcc -Wall -Wextra -pthread -g tests/concurrent_test.c -o tests/concurrent_test
./tests/concurrent_test
```
## Test Results

```
  Test Results Summary
Total requests sent:      100
Successful requests:      100
Failed requests:          0
Success rate:             100.0%
Total test time:          262.32 ms
Average latency:          3.09 ms
Throughput:               381.2 requests/sec
```

## Configuration

Edit these constants in `threadpool.h`:
- `DEFAULT_THREAD_COUNT` - Number of worker threads (default: 4)
- `MAX_QUEUE_SIZE` - Maximum pending connections (default: 256)

Edit port in `main.c`:
- `port` variable (default: 8081)