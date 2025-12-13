// handler.c — HTTP request handler module
// Jide: temporary Sprint 0 stub
// Osa: full HTTP logic in Sprint 2
//

#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RECV_BUFFER 4096

static int send_all(int fd, const char *buf, size_t len){
    size_t total = 0;

    while (total < len){
        ssize_t sent = send(fd, buf+total, len-total, 0); 
        if (sent <= 0){
            return -1;
        }
        total += sent;
    }
    return 0;
}

static const char *get_mime_type(const char* path){
    const char *ext= strrchr(path, '.');
    if(!ext) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css")  == 0) return "text/css";
    if (strcmp(ext, ".js")   == 0) return "application/javascript";
    if (strcmp(ext, ".png")  == 0) return "image/png";
    if (strcmp(ext, ".jpg")  == 0) return "image/jpeg";
    if (strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".txt")  == 0) return "text/plain";

    return "application/octet-stream";
}

static void send_http_response( int fd, int status, const char *status_text, const char *content_type, const char *body){
    char header[512];
    int header_len = snprintf(header, sizeof(header), "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n", status, status_text, content_type, strlen(body));

    send_all(fd, header, header_len);
    send_all(fd, body, strlen(body));
}

static void serve_file(int fd, const char *path){
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s%s", "public", path);

    int file = open(fullpath, O_RDONLY);
    if (file < 0){
        const char *msg = "<h1>404 Not Found</h1>";
        send_http_response(fd, 404, "Not Found", "text/html", msg);
        return;
    }

    struct stat standard;
    if (fstat(file, &standard) < 0)
    {
        close(file);
        const char *msg = "<h1>500 Internal Server Error</h1>";
        send_http_response(fd, 500, "Internal Server Error", "text/html", msg);
        return;
    }

    size_t filesize = standard.st_size;
    char* buffer = malloc(filesize);

    if (!buffer){
        close(file);
        const char *msg = "<h1>500 Internal Server Error</h1>";
        send_http_response(fd, 500, "Internal Server Error", "text/html", msg);
        return;
    }

    ssize_t bytes = read(file, buffer, filesize);
    close(file);


    if (bytes != (ssize_t)filesize){
        free(buffer);
        const char *msg = "<h1>500 Internal Server Error</h1>";
        send_http_response(fd, 500, "Internal Server Error", "text/html", msg);
        return;
    }

    const char *mime = get_mime_type(path);


    char header[512];
    int header_len = snprintf(header, sizeof(header), 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %zu\r\n"
    "Connection: close\r\n\r\n", mime, filesize);

    send_all(fd, header, header_len);
    send_all(fd, buffer, filesize);

    free(buffer);

}



void handle_connection_stub(int client_file_descriptor) {
    char buffer[RECV_BUFFER];

    ssize_t bytes = recv(client_file_descriptor, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) {
        perror("Failed to receive data from client");
        close(client_file_descriptor);
        return;
    }

    if (bytes == 0)
    {
        printf("Connection closed by client.\n");
        close(client_file_descriptor);
        return;
    }

    buffer[bytes] = '\0';
    printf("Received %zd bytes from client:\n%s\n", (int)bytes, buffer);


    char method[8], path[256];
    if (sscanf(buffer, "%7s %255s", method, path) != 2){
        send_http_response(client_file_descriptor, 400, "Bad Request", "text/html", "<h1>Bad Request</h1>");
        close(client_file_descriptor);
        return;
    }

    if (strcmp(method, "GET") != 0)
    {
        const char *msg = "<h1>Method Not Allowed</h1>";
        send_http_response(client_file_descriptor, 405, "Method Not Allowed", "text/html", msg);

        close(client_file_descriptor);
        return;
    }

    if (strcmp(path, "/") == 0){
        strcpy(path, "/index.html");

    }

    printf("Serving file for path: %s\n", path); 
    
    serve_file(client_file_descriptor, path);

    printf("Finished serving client. Closing connection.\n");

    close(client_file_descriptor);
}   




// Sprint 0 - basic test response
    // const char *msg =
    //     "HTTP/1.0 200 OK\r\n"
    //     "Content-Type: text/plain\r\n"
    //     "\r\n"
    //     "Hello from Sprint 0 handler!\n";

    // ssize_t sentMsg = send(client_file_descriptor, msg, strlen(msg), 0);

    // if (sentMsg < 0){
    //     perror("send() failed");
    //     close(client_file_descriptor);
    //     return;
    // }

    // printf("Response sent to client, closing connection\n");
    // close(client_file_descriptor);