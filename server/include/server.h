#ifndef SERVER_H
#define SERVER_H

#define _GNU_SOURCE

#include <poll.h>

#include "../../shared/include/utils.h"

#define PORT 1337
#define MAX_MESSAGE_SIZE 1400

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
