#include "../include/core.h"

void parse_argvs(int argc, char **argv, char *dst_ip, char *src_ip) {
    switch (argc) {
        case 1:
            print_usage();
            exit(EXIT_FAILURE);

        case 2:
            print_usage();
            exit(EXIT_FAILURE);

        case 3:
            if (strcmp(argv[1], "-d") == 0) {
                strncpy(dst_ip, argv[2], IP_SIZE);
                strncpy(src_ip, get_src_ip(), IP_SIZE);

                return;
            }
            else {
                print_usage();
                exit(EXIT_SUCCESS);
            }

        case 4:
            print_usage();
            exit(EXIT_FAILURE);

        case 5:
            if ((strcmp(argv[1], "-d") == 0) && (strcmp(argv[3], "-s") == 0)) {
                strncpy(dst_ip, argv[2], IP_SIZE);
                strncpy(src_ip, argv[4], IP_SIZE);

                return;
            }
            else {
                print_usage();
                exit(EXIT_SUCCESS);
            }

        default:
            print_usage();
            exit(EXIT_FAILURE);
    }
}

void print_usage() {
    printf("\nusage syntax: ./client -d [destination address] -s [source address]>\n");
    printf(" - destination address must be provided\n");
    printf(" - source address is optional\n\n");
}

char *get_host_name() {
    char* host_name = NULL;

    host_name = calloc(MAX_HOSTNAME_SIZE, sizeof(char));
    if (host_name == NULL) {
        perror("Client: calloc(host name)");
        exit(EXIT_FAILURE);
    }

    gethostname(host_name, MAX_HOSTNAME_SIZE);
    return host_name;
}

char *get_src_ip() {
    char* host_name = NULL;
    struct hostent* hst = NULL;

    host_name = get_host_name();

    gethostname(host_name, MAX_HOSTNAME_SIZE);
    hst = gethostbyname();

    free(host_name);
    return inet_ntoa(*(struct in_addr *)hst->h_addr);
}

void init_socket(int *sd, struct sockaddr_in *dst_addr, char *dst_ip, char *dst_port) {
    socklen_t s_len = 0;

    s_len = sizeof(struct sockaddr_in);
    memset(dst_addr, 0, s_len);

    *sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sd == -1) {
        perror("Client: socket(dest_ip)");

        close(*sd);
        exit(EXIT_FAILURE);
    }

    dst_addr->sin_family = AF_INET;
    dst_addr->sin_addr.s_addr = inet_addr(dst_ip);
    dst_addr->sin_port = htons(dst_port);
}

unsigned char connect(int sd, struct sockaddr_in *dst_addr) {
    unsigned char p_id = 0;
    int epd = 0, rtn = 0;
    struct epoll_event ev, *events = NULL;
    char msg[MAX_MSG_SIZE];
    socklen_t s_len = 0;
    ssize_t bts = 0;

    s_len = sizeof(struct sockaddr_in);

    memset(msg, 0, MAX_MSG_SIZE);
    memset(ev, 0, sizeof(struct epoll_event));
    memset(events, 0, sizeof(struct epoll_event));

    events = calloc(1, sizeof(struct epoll_event));
    if (events == NULL) {
        perror("Client: calloc(events)");
        exit(EXIT_FAILURE);
    }

    epd = epoll_create(1);
    if (epd == -1) {
        perror("Client: epoll_create");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = sd;

    rtn = epoll_ctl(epd, EPOLL_CTL_ADD, sd, &ev);
    if (rtn == -1) {
        perror("Client: epoll_ctl(sd)");
        exit(EXIT_FAILURE);
    }

    strncpy(msg, "Hello, bomberman server!", strlen("Hello, bomberman server!"));

    bts = sendto(sd_cln, msg, strlen(msg), 0, (struct sockaddr*)&dst_addr, s_len);
    if (bts == -1) {
        perror("Client: sendto(dest_ip)");

        close(sd_cln);
        exit(EXIT_FAILURE);
    }

    rtn = epoll_wait(epd, events, 1, MAX_WAIT_TIME);
    if (rtn == -1) {
        perror("Client: epoll_wait()");
        exit(EXIT_FAILURE);
    }
    else
        if (rtn == 0) {
            perror("Client: time exceeded");
            exit(EXIT_FAILURE);
        }

    s_len = sizeof(struct sockaddr_in);

    bts = recvfrom(sd_cln, msg, MAX_MSG_SIZE, 0, (struct sockaddr*)dst_addr, &s_len);
    if (bts == -1) {
        perror("Client: recvfrom(dest_ip)");
        exit(EXIT_FAILURE);
    }

    p_id = msg[0];

    s_len = sizeof(struct sockaddr_in);

    bts = recvfrom(sd_cln, map, MAP_H * MAP_W, 0, (struct sockaddr*)dst_addr, &s_len);
    if (bts == -1) {
        perror("Client: recvfrom(dest_ip)");
        exit(EXIT_FAILURE);
    }
}
