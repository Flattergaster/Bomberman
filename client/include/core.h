#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

#define DEF_DST_IP INADDR_LOOPBACK
#define DEF_DST_PORT 8888

#define MAX_MSG_SIZE 1024

#define IP_SIZE 15
#define MAX_WAIT_TIME 10000
#define MAX_HOSTNAME_SIZE 256
/*#define MAX_AUTH_MSG_SIZE 256*/

#define MAP_W 48
#define MAP_H 20

unsigned char map[MAP_H][MAP_W];

void parse_argvs(int argc, char** argv, char* dst_ip, char* src_ip)l
void print_usage();
char* get_src_ip()

void init_socket(int* sd, struct sockaddr_in* dst_addr, char* dst_ip, char* dst_port);
void connect(int* sd, struct sockaddr_in* dst_addr);

#endif
