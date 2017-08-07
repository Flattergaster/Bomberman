#ifndef CORE_H
#define CORE_H

#define _GNU_SORCE

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
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

#include "gui.h"
#include "shared/include/utils.h"
#include "shared/include/constants.h"

#define DEF_DST_IP "192.168.2.1"
#define DEF_DST_PORT 1337
#define DEF_SRC_IP "192.168.2.83"
#define DEF_SRC_PORT 8888

#define IP_SIZE 15
#define PORT_SIZE 6
#define MAX_KEY_MSG 64
#define MAX_HOSTNAME_SIZE 256
#define MAX_WAIT_TIME 10000

#define KEY_SPACE_N 32
#define KEY_ESC 27

typedef struct _connect_info {
    int sd;
    uint8_t p_id;
    struct sockaddr_in dst_addr;
    surface_t *surface;
    pthread_t *ctl_hndl_tid;
    pthread_t *recv_hndl_tid;
} connect_info_t;

unsigned char map[MAP_H][MAP_W];

void parse_argvs(int argc, char **argv, uint8_t *dst_ip, uint16_t *dst_port,
                    uint8_t *src_ip, uint16_t *src_port);
void print_usage();

char *get_host_name();
char *get_src_ip();

void sig_hndl(int sig_num);
void init_signal(struct sigaction *sig_act);

void init_socket(int *sd, struct sockaddr_in *dst_addr,
                    uint8_t *dst_ip, uint16_t *dst_port);
void init_connect(int sd, struct sockaddr_in *dst_addr,
                    pthread_t *ctl_hndl_tid, pthread_t* recv_hndl_tid,
                    surface_t *surface, connect_info_t *c_info);

void *control_hndl(void* args);
void *recv_hndl(void* args);

#endif
