# Multithreaded Web Server

A high-performance HTTP/1.0 web server implemented in C that handles multiple client requests concurrently using POSIX threads.

**Course:** Systems Programming  
**Repository:** https://github.com/Lajaiyd/multithreaded-web-server

## Team Members

| Name |
|------|
| Olajide Akinyemi|
| Pratik Kamble |
| Osaretinmwen Oriakhi |

---

## Project Description

This project implements a **Multithreaded Web Server** capable of handling multiple client requests concurrently using POSIX threads. The server implements core components of a basic HTTP/1.0 server including:

- TCP socket setup and connection handling
- Thread pool management with configurable worker threads
- Synchronized request queue using mutex and condition variables
- Static file serving with MIME type detection
- Graceful shutdown functionality

The architecture follows a producer-consumer model where the main thread accepts connections and enqueues them, while worker threads dequeue and process requests concurrently.

---

## Key Features

- **Concurrent Connection Handling:** Thread pool with 4 worker threads (configurable)
- **Synchronized Request Queue:** Thread-safe queue using pthread mutex and condition variables
- **HTTP/1.0 Support:** GET method with proper request parsing
- **Static File Serving:** Serves HTML, CSS, JS, images, and text files
- **MIME Type Detection:** Automatic content-type headers based on file extension
- **Error Handling:** Proper HTTP error responses (400, 404, 405, 500)
- **Graceful Shutdown:** Signal handlers for clean termination (Ctrl+C)
- **Port Reuse:** SO_REUSEADDR for quick server restarts

---

## Build and Run Instructions

### Prerequisites

- GCC compiler
- POSIX-compliant system (Linux/macOS)
- pthread library

### Building the Project

```bash
# Clone the repository
git clone https://github.com/Lajaiyd/multithreaded-web-server.git
cd multithreaded-web-server

# Clean and build
make clean
make
```

### Running the Server

```bash
# Start the server
./bin/server

# Or use make
make run
```

**Expected Output:**
```
Initializing thread pool with 4 workers...
[ThreadPool] Initializing with 4 worker threads
[Worker 0] Started
[Worker 1] Started
[Worker 2] Started
[Worker 3] Started
[ThreadPool] Successfully initialized with 4 workers
Server started and listening on port 8081
Starting server main loop (Sprint 1 - Thread Pool)...
```

### Stopping the Server

Press `Ctrl+C` for graceful shutdown:
```
^C
Received signal 2, shutting down...
[ThreadPool] Initiating shutdown...
[Worker 0] Shutting down
[Worker 1] Shutting down
[Worker 2] Shutting down
[Worker 3] Shutting down
[ThreadPool] Shutdown complete
```

---

## Usage Guidelines

### Accessing the Server

The server listens on **port 8081** by default.

**Using a Web Browser:**
- http://localhost:8081/ (serves index.html)
- http://localhost:8081/about.html
- http://localhost:8081/readme.txt

**Using curl:**
```bash
curl http://localhost:8081/
curl http://localhost:8081/about.html
curl http://localhost:8081/readme.txt
```

### Adding Static Files

Place files in the `public/` directory:
```
public/
├── index.html      (default page for "/" requests)
├── about.html
├── readme.txt
└── (add your files here)
```

### Supported MIME Types

| Extension | Content-Type |
|-----------|--------------|
| .html | text/html |
| .css | text/css |
| .js | application/javascript |
| .png | image/png |
| .jpg, .jpeg | image/jpeg |
| .txt | text/plain |

### Running Concurrent Tests

```bash
# Compile the test
gcc -Wall -Wextra -pthread -g tests/concurrent_test.c -o tests/concurrent_test

# Run with server active
./tests/concurrent_test
```

---

## Project Structure

```
multithreaded-web-server/
├── Makefile
├── README.md
├── include/
│   ├── server.h          # Socket server declarations
│   ├── threadpool.h      # Thread pool declarations
│   └── handler.h         # HTTP handler declarations
├── src/
│   ├── main.c            # Entry point, initialization
│   ├── server.c          # Socket setup, accept loop
│   ├── threadpool.c      # Thread pool implementation
│   └── handler.c         # HTTP parsing, file serving
├── public/
│   ├── index.html        # Default homepage
│   ├── about.html        # About page
│   └── readme.txt        # Sample text file
├── tests/
│   └── concurrent_test.c # Concurrent client test
└── bin/
    └── server            # Compiled binary
```

---

## Team Member Contributions

### Olajide Akinyemi (Socket Layer)

**Files:** `src/server.c`, `include/server.h`, `src/main.c` (initial), `Makefile`, `architecture_diagram.md`

**Responsibilities:**
- Designed overall server architecture and project structure
- Implemented TCP socket setup (`socket()`, `bind()`, `listen()`)
- Created client connection acceptance with IP logging
- Implemented the main accept loop structure
- Set up `SO_REUSEADDR` for port reuse
- Created Makefile with build targets
- Documented architecture with flowchart diagram

**Key Functions:**
| Function | Description |
|----------|-------------|
| `start_server()` | Creates and configures the listening socket |
| `accept_connection()` | Accepts incoming client connections |
| `main_accept_loop()` | Main server loop (initial version) |

---

### Pratik Kamble (Thread Pool Layer)

**Files:** `src/threadpool.c`, `include/threadpool.h`, `src/main.c` (updated), `src/server.c` (integration), `tests/concurrent_test.c` (initial)

**Responsibilities:**
- Designed and implemented complete thread pool architecture
- Implemented synchronized request queue with mutex + condition variables
- Created worker thread initialization and routines
- Implemented graceful shutdown mechanism with signal handlers
- Integrated thread pool with existing server code
- Updated main accept loop to use `enqueue_client()`
- Created initial concurrent stress test

**Key Functions:**
| Function | Description |
|----------|-------------|
| `threadpool_init()` | Initializes thread pool with N workers |
| `enqueue_client()` | Adds client fd to synchronized queue |
| `threadpool_shutdown()` | Graceful shutdown of all workers |
| `worker_routine()` | Worker thread function (dequeue + handle) |
| `queue_push()` | Thread-safe enqueue (blocks if full) |
| `queue_pop()` | Thread-safe dequeue (blocks if empty) |

**Synchronization Primitives:**
| Primitive | Purpose |
|-----------|---------|
| `pthread_mutex_t` | Protects queue access |
| `pthread_cond_t not_empty` | Workers wait when queue is empty |
| `pthread_cond_t not_full` | Main thread waits when queue is full |

---

### Osaretinmwen Oriakhi (HTTP Handler Layer)

**Files:** `src/handler.c`, `include/handler.h`, `public/*` (static files), `tests/concurrent_test.c` (updated)

**Responsibilities:**
- Implemented HTTP request parsing (method, path extraction)
- Implemented static file serving from `public/` directory
- Added MIME type detection for various file extensions
- Implemented HTTP response generation with proper headers
- Created error responses (400 Bad Request, 404 Not Found, 405 Method Not Allowed, 500 Internal Server Error)
- Created sample static files for testing
- Simplified concurrent test client

**Key Functions:**
| Function | Description |
|----------|-------------|
| `handle_connection_stub()` | Main request handler (parsing + routing) |
| `serve_file()` | Reads and sends file contents |
| `send_http_response()` | Generates HTTP response with headers |
| `get_mime_type()` | Maps file extension to MIME type |
| `send_all()` | Reliable send (handles partial writes) |

**HTTP Features:**
- GET method support
- Path parsing and routing
- Default `index.html` for "/" path
- Content-Type and Content-Length headers

---

## Test Results

### Concurrent Test (20 simultaneous clients)

```
Starting concurrent test with 20 clients...
[Client 0] Received response (xxx bytes)
[Client 1] Received response (xxx bytes)
...
[Client 19] Received response (xxx bytes)
Concurrent test complete.
```

### Performance Metrics

| Metric | Value |
|--------|-------|
| Total Requests | 100 |
| Success Rate | 100% |
| Average Latency | ~3 ms |
| Throughput | ~380 requests/sec |

---

### Screenshots

<img width="947" height="521" alt="Screenshot 2025-12-13 at 19 24 01" src="https://github.com/user-attachments/assets/feec90fa-88e2-4ce9-ac28-39910025fb4f" />

<img width="1920" height="1080" alt="Screenshot 2025-12-14 at 16 15 27" src="https://github.com/user-attachments/assets/025b3c69-34b7-4205-b721-c7ef9b64b2db" />

<img width="1919" height="1080" alt="Screenshot 2025-12-14 at 16 15 40" src="https://github.com/user-attachments/assets/1b40ddce-5236-4025-83cd-436fdae9f5ad" />








