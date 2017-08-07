#include "../include/core.h"

void parse_argvs(int argc, char **argv, uint8_t *dst_ip, uint16_t *dst_port,
                        uint8_t *src_ip, uint16_t *src_port) {
    switch (argc) {
        case 1:
            print_usage();
            log_error(stdout, "too few arguments to initialize the client");

            exit(EXIT_FAILURE);

        case 2:
            if (strcmp(argv[1], "-def") == 0) {
                strncpy((char*)dst_ip, DEF_DST_IP, IP_SIZE);
                *dst_port = DEF_DST_PORT;

                strncpy((char *)src_ip, get_src_ip(), IP_SIZE);
                *src_port = DEF_SRC_PORT;

                return;
            }

            print_usage();
            log_error(stdout, "invalid arguments syntax to initialize the client");

            exit(EXIT_FAILURE);

        case 3:
            if (strcmp(argv[1], "-d") == 0)
                if (sscanf(argv[2], "%[^:]:%hu", dst_ip, dst_port) == 2) {
                    strncpy((char *)src_ip, get_src_ip(), IP_SIZE);
                    *src_port = DEF_SRC_PORT;

                    return;
                }

            print_usage();
            log_error(stdout, "invalid arguments syntax to initialize the client");

            exit(EXIT_FAILURE);

        case 4:
            print_usage();
            exit(EXIT_FAILURE);

        case 5:
            if ((strcmp(argv[1], "-d") == 0) && (strcmp(argv[3], "-s") == 0))
                if ((sscanf(argv[2], "%[^:]:%hu", dst_ip, dst_port) == 2) &&
                    (sscanf(argv[4], "%[^:]:%hu", src_ip, src_port) == 2))
                    return;

            print_usage();
            log_error(stdout, "invalid arguments syntax to initialize the client");

            exit(EXIT_FAILURE);

        default:
            print_usage();
            log_error(stdout, "too few or invalid arguments syntax to initialize the client");

            exit(EXIT_FAILURE);
    }
}

void print_usage() {
    printf("usage syntax: ./client [-def] -d [destination address]:[port] -s [source address]:[port]\n");
    printf(" --- destination address must be provided\n");
    printf(" --- source address is optional\n\n");
}

char *get_host_name() {
    int rtn = 0;
    char* host_name = NULL;

    host_name = calloc(MAX_HOSTNAME_SIZE, sizeof(char));
    if (host_name == NULL) {
        log_error(stdout, "calloc(host name)");
        exit(EXIT_FAILURE);
    }

    rtn = gethostname(host_name, MAX_HOSTNAME_SIZE);
    return (rtn == 0) ?  host_name : NULL;
}

char *get_src_ip() {
    char* host_name = NULL;
    struct hostent* hst = NULL;

    host_name = get_host_name();
    if (host_name == NULL) {
        log_error(stdout, "gethostname/get_host_name");
        exit(EXIT_FAILURE);
    }

    hst = gethostbyname(host_name);
    if (hst == NULL) {
        free(host_name);

        log_error(stdout, "gethostbyname");
        exit(EXIT_FAILURE);
    }

    free(host_name);
    return inet_ntoa(*(struct in_addr *)hst->h_addr);
}

void sig_hndl(int sig_num) {
    return;
}

void init_signal(struct sigaction *sig_act) {
    int rtn = 0;

    sig_act->sa_handler = sig_hndl;
    sigemptyset(&sig_act->sa_mask);
    sig_act->sa_flags = 0;

    rtn = sigaction(SIGINT, sig_act, NULL);
    if (rtn == -1) {
        log_error(stdout, "sigaction");
        exit(EXIT_FAILURE);
    }

    log_notice(stdout, "initialization signals (ctrl + c) processing");
}

void init_socket(int *sd, struct sockaddr_in *dst_addr, uint8_t *dst_ip,
                        uint16_t *dst_port) {
    socklen_t s_len = 0;

    s_len = sizeof(struct sockaddr_in);
    memset(dst_addr, 0, s_len);

    *sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sd == -1) {
        log_error(stdout, "socket(dest_ip)");

        close(*sd);
        exit(EXIT_FAILURE);
    }

    log_notice(stdout, "initialization socket");

    dst_addr->sin_family = AF_INET;
    dst_addr->sin_addr.s_addr = inet_addr((char*)dst_ip);
    dst_addr->sin_port = htons(*dst_port);

    log_notice(stdout, "initialization sockadrr_in");
}

void init_connect(int sd, struct sockaddr_in *dst_addr, pthread_t *ctl_hndl_tid, pthread_t *recv_hndl_tid, surface_t *surface, connect_info_t *c_info) {
    uint8_t p_id = 0, msg[MAX_MSG_SIZE];
    int epd = 0, rtn = 0;
    struct epoll_event ev, *events = NULL;
    socklen_t s_len = 0;
    ssize_t bts = 0;

    log_notice(stdout, "connecting to the server");

    s_len = sizeof(struct sockaddr_in);

    memset(msg, 0, MAX_MSG_SIZE);
    memset(&ev, 0, sizeof(struct epoll_event));

    events = calloc(1, sizeof(struct epoll_event));
    if (events == NULL) {
        log_error(stdout, "calloc(events)");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }

    epd = epoll_create(1);
    if (epd == -1) {
        log_error(stdout, "epoll_create");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = sd;

    rtn = epoll_ctl(epd, EPOLL_CTL_ADD, sd, &ev);
    if (rtn == -1) {
        log_error(stdout, "epoll_ctl(sd)");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }

    strncpy((char*)msg, "Hello, bomberman server!", strlen("Hello, bomberman server!"));

    bts = sendto(sd, msg, strlen((char*)msg), 0, (struct sockaddr*)dst_addr, s_len);
    if (bts == -1) {
        log_error(stdout, "sendto(dest_ip)");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }

    log_notice(stdout, "sending a welcome message to the server");

    rtn = epoll_wait(epd, events, 1, MAX_WAIT_TIME);
    if (rtn == -1) {
        log_error(stdout, "epoll_wait");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }
    else
        if (rtn == 0) {
            log_error(stdout, "time exceeded");

            del_surf(&surface);
            close(sd);

            exit(EXIT_FAILURE);
        }

    s_len = sizeof(struct sockaddr_in);
    memset(dst_addr, 0, s_len);

    bts = recvfrom(sd, &p_id, sizeof(uint8_t), 0, (struct sockaddr*)dst_addr, &s_len);
    if (bts == -1) {
        log_error(stdout, "recvfrom(dest_ip)");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }

    if (p_id < 200 || p_id > 210) {
        log_error(stdout, "ivalid player id");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }

    log_notice(stdout, "reciving the user ID from the server");

    s_len = sizeof(struct sockaddr_in);
    memset(dst_addr, 0, s_len);

    bts = recvfrom(sd, map, MAP_H * MAP_W, 0, (struct sockaddr*)dst_addr, &s_len);
    if (bts == -1) {
        log_error(stdout, "recvfrom(dest_ip)");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }
    log_notice(stdout, "reciving the map");

    print_map(surface, p_id);
    log_notice(stdout, "print map");

    strncpy((char*)msg, "Connection completed successfully", strlen("Connection completed successfully"));

    bts = sendto(sd, msg, strlen((char*)msg), 0, (struct sockaddr*)dst_addr, s_len);
    if (bts == -1) {
        log_error(stdout, "sendto(dest_ip)");

        del_surf(&surface);
        close(sd);

        exit(EXIT_FAILURE);
    }
    log_notice(stdout, "sending a connection completed message to the server");

    c_info->sd = sd;
    c_info->dst_addr = *dst_addr;
    c_info->p_id = p_id;
    c_info->surface = surface;
    c_info->ctl_hndl_tid = ctl_hndl_tid;
    c_info->recv_hndl_tid = recv_hndl_tid;

    log_notice(stdout, "initialization struct connect_info_t");

    free(events);
}

void *control_hndl(void* args) {
    int key = 0, rtn = 0;
    uint8_t send_key = 0;
    connect_info_t *c_info = NULL;
    socklen_t s_len = 0;
    ssize_t bts = 0;

    s_len = sizeof(struct sockaddr_in);

    if (args == NULL) {
        log_error(stdout, "args is NULL");
        exit(EXIT_FAILURE);
    }

    c_info = (connect_info_t *)args;

    do {
        key = getch();
        send_key = 0;

        switch(key) {
            case KEY_UP:
                send_key = KEY_U;
                break;

            case KEY_DOWN:
                send_key = KEY_D;
                break;

            case KEY_LEFT:
                send_key = KEY_L;
                break;

            case KEY_RIGHT:
                send_key = KEY_R;
                break;

            case KEY_SPACE_N:
                send_key = KEY_S;
                break;

            case KEY_ESC:
                send_key = KEY_E;
                break;
        }

        if (send_key != 0) {
            bts = sendto(c_info->sd, &send_key, sizeof(uint8_t), 0, (struct sockaddr*)&c_info->dst_addr, s_len);
            if (bts == -1) {
                log_error(stdout, "sendto(dest_ip)");
                exit(EXIT_FAILURE);
            }

            if (send_key == KEY_E) {
                rtn = pthread_cancel(*c_info->recv_hndl_tid);
                if (rtn != 0) {
                    log_error(stdout, "pthread_cancel(recv_hndl_tid)");
                    exit(EXIT_FAILURE);
                }

                pthread_exit(NULL);
            }
        }

    } while (1);
}

void *recv_hndl(void* args) {
    int rtn = 0;
    connect_info_t *c_info = NULL;
    ssize_t bts = 0;

    if (args == NULL) {
        log_error(stdout, "args is NULL");
        exit(EXIT_FAILURE);
    }

    c_info = (connect_info_t *)args;

    do {
        bts = recvfrom(c_info->sd, map, MAP_H * MAP_W, 0, NULL, NULL);
        if (bts == -1) {
            log_error(stdout, "recvfrom(dest_ip)");
            pthread_exit(NULL);
        }

        if (strncmp((char*)&map[0], "Game Over!", strlen("Game Over!")) == 0) {
            rtn = pthread_cancel(*c_info->ctl_hndl_tid);
            if (rtn != 0) {
                log_error(stdout, "pthread_cancel(ctl_hndl_tid)");
                exit(EXIT_FAILURE);
            }

            pthread_exit(NULL);
        }

        print_map(c_info->surface, c_info->p_id);
    } while(1);
}
