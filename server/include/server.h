#ifndef SERVER_H
#define SERVER_H

#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE

#include <poll.h>

#include "../include/game.h"
#include "../../shared/include/utils.h"

#define PORT 1337
#define MAX_MESSAGE_SIZE 1400

#define MAX_PORT_VALUE 65535
#define MAX_PLAYERS 10

int create_socket(struct sockaddr_in *addr, uint16_t port);
int create_player(int sd, struct sockaddr_in client_addr);
int listener_new_clients(int sd);
int accept_player(int sd, struct sockaddr_in *addr, 
                            socklen_t *addr_len, int p_id);
int kill_player(int index);
void *client_thread(void *args);
int do_action(int index, uint8_t key);
void broadcast_map();

#endif
