#ifndef CORE_H
#define CORE_H

#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _POSIX_C_SOURCE
#define _XOPEN_SOURCE
#define _SVID_SOURCE
#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>

#include <netdb.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

#include "gui.h"

#define DEF_DST_IP "192.168.2.1"
#define DEF_DST_PORT 1337
#define DEF_SRC_IP "192.168.2.83"
#define DEF_SRC_PORT 8888

#define IP_SIZE 15
#define PORT_SIZE 6
#define MAX_KEY_MSG 64
#define MAX_HOSTNAME_SIZE 256
#define MAX_MSG_SIZE 1024
#define MAX_WAIT_TIME 10000

#define KEY_SPACE_N 32
#define KEY_ESC 27

#define KEY_L 1
#define KEY_R 2
#define KEY_U 3
#define KEY_D 4
#define KEY_S 5
#define KEY_E 6

typedef struct _connect_info {
    int sd;
    struct sockaddr_in dst_addr;
} connect_info_t;

unsigned char map[MAP_H][MAP_W];

void parse_argvs(int argc, char **argv, uint8_t *dst_ip, uint16_t *dst_port, uint8_t *src_ip, uint16_t *src_port);
void print_usage();

char *get_host_name();
char *get_src_ip();

void init_socket(int *sd, struct sockaddr_in *dst_addr, uint8_t *dst_ip, uint16_t *dst_port);
uint8_t init_connect(int sd, struct sockaddr_in *dst_addr);

void *control_hndl(void* args);

#endif
