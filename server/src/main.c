#include "../include/server.h"
#include "../include/game.h"

int main() {
    struct sockaddr_in addr_server;
    int sd = 0, status = 0;
    
    sd = create_socket(&addr_server, SERVER_PORT);
    if (sd < 0) {
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));
    memset(players, 0, MAX_PLAYERS);
    generate_map();
   
    status = listener_new_clients(sd);
    if (status < 0)  {
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
