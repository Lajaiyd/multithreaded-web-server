//
// Created by Olajide Akinyemi on 11/25/25.
//

#ifndef HANDLER_H
#define HANDLER_H

// Handle a single client connection.
// Sprint 0: simple echo / test response
// Sprint 2: full HTTP request parsing + file serving
void handle_connection_stub(int client_file_descriptor);

static int send_all(int fd, const char *buf, size_t len);

static const char *get_mime_type(const char* path);

static void send_http_response( int fd, int status, const char *status_text, const char *content_type, const char *body);

static void serve_file(int fd, const char *path);



#endif
