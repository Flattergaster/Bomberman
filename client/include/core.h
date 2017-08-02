#ifndef CORE_H
#define CORE_H

#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _POSIX_C_SOURCE
#define _XOPEN_SOURCE
#define _SVID_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>

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

#define DEF_DST_IP "192.168.2.1"
#define DEF_DST_PORT 1337

#define DEF_SRC_IP "192.168.2.83"
#define DEF_SRC_PORT 8888

#define MAX_MSG_SIZE 1024

#define IP_SIZE 15
#define PORT_SIZE 6
#define MAX_WAIT_TIME 10000
#define MAX_HOSTNAME_SIZE 256
/*#define MAX_AUTH_MSG_SIZE 256*/

#define MAP_W 48
#define MAP_H 20

unsigned char map[MAP_H][MAP_W];

void parse_argvs(int argc, char **argv, unsigned char *dst_ip, unsigned  short int *dst_port, unsigned char *src_ip, unsigned short int *src_port);
void print_usage();

char *get_host_name();
char *get_src_ip();

void init_socket(int *sd, struct sockaddr_in *dst_addr, unsigned char *dst_ip, unsigned short int *dst_port);
unsigned char init_connect(int sd, struct sockaddr_in *dst_addr);

#endif
