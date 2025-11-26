flowchart

    A[Client Request<br/>Browser / curl] -->|TCP Connection| B[Socket Server<br/>Jide]
    
    subgraph Jide[Socket Layer]
        B1[socket()] --> B2[bind()]
        B2 --> B3[listen()]
        B3 --> B4[accept()]
    end
    
    B4 --> C[Incoming client_fd]

    subgraph PK[Thread Pool Layer]
        C --> D[enqueue(client_fd)]
        D --> E[Worker Thread<br/>pthread]
    end

    E --> F[handle_connection(client_fd)]

    subgraph Osa[HTTP Handler Layer]
        F1[Parse HTTP Request] 
        F2[Locate File / Route] 
        F3[Send Response]
    end

    F --> F1 --> F2 --> F3 --> G[Response sent back to client]
