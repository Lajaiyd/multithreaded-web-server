#include "server.h"
#include <unistd.h>

int main(void) {
    int port = 8080;
    int server_file_descriptor = start_server(port);

    main_accept_loop(server_file_descriptor);

    close(server_file_descriptor);
    return 0;
}
