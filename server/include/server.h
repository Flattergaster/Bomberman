#ifndef SERVER_H
#define SERVER_H

#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#include "../../shared/include/utils.h"

#define PORT 1337
#define MAX_PORT_VALUE 65535
#define MAX_PLAYERS 10

typedef struct _player {
    int p_id;
    int x;
    int y;
    int bomb_str;
    int bomb_pwr;
    int sd;
    struct sockaddr_in end_point;
} player_t;

extern player_t players[];

int create_socket(struct sockaddr_in *addr, uint16_t port);
int create_player(int sd, struct sockaddr_in client_addr);
int listener_new_clients(int sd);
void *client_thread(void *args);
#endif
