//
// Created by Olajide Akinyemi on 11/25/25.
//

#ifndef SERVER_H
#define SERVER_H

int start_server(int port);
int main_accept_loop(int server_file_descriptor);
int accept_connections(int server_file_descriptor);

#endif // SERVER_H