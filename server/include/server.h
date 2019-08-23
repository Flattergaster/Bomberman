#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>

#include "utils.h"
#include "constants.h"

#include "game.h"
#include "bomb.h"

#define MAX_PORT_VALUE 65535

int create_socket(struct sockaddr_in *addr, uint16_t port);
int create_player(int sd, struct sockaddr_in client_addr);
int listener_new_clients(int sd);
int accept_player(int sd, struct sockaddr_in *addr, 
                            socklen_t *addr_len, int p_id);
void *client_thread(void *args);
int do_action(int index, uint8_t key);

int start_buff_spawner();

#endif
