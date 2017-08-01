#include "../include/server.h"

int main() {
    struct sockaddr_in addr_server;
    int sd = 0, status = 0;
    
    sd = create_socket(&addr_server, PORT);
    if (sd < 0) {
        exit(EXIT_FAILURE);
    }
    
    memset(players, 0, MAX_PLAYERS);
    
    status = listener_new_clients(sd);
    if (status < 0)  {
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
