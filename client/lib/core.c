#include "../include/core.h"
#include "../include/gui.h"

void parse_argvs(int argc, char **argv, uint8_t *dst_ip, uint16_t *dst_port,
                        uint8_t *src_ip, uint16_t *src_port) {
    switch (argc) {
        case 1:
            print_usage();
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
            exit(EXIT_FAILURE);

        case 3:
            if (strcmp(argv[1], "-d") == 0)
                if (sscanf(argv[2], "%[^:]:%hu", dst_ip, dst_port) == 2) {
                    strncpy((char *)src_ip, get_src_ip(), IP_SIZE);
                    *src_port = DEF_SRC_PORT;

                    return;
                }

            print_usage();
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
            exit(EXIT_SUCCESS);

        default:
            print_usage();
            exit(EXIT_FAILURE);
    }
}

void print_usage() {
    printf("[Error] invalid argument syntax\n");
    printf("usage syntax: ./client [-def] -d [destination address]:[port] -s [source address]:[port]\n");
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
    hst = gethostbyname(host_name);

    free(host_name);
    return inet_ntoa(*(struct in_addr *)hst->h_addr);
}

void init_socket(int *sd, struct sockaddr_in *dst_addr, uint8_t *dst_ip,
                        uint16_t *dst_port) {
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
    dst_addr->sin_addr.s_addr = inet_addr((char*)dst_ip);
    dst_addr->sin_port = htons(*dst_port);
}

uint8_t init_connect(int sd, struct sockaddr_in *dst_addr, surface_t *surface, connect_info_t *c_info) {
    uint8_t p_id = 0, msg[MAX_MSG_SIZE];
    int epd = 0, rtn = 0;
    struct epoll_event ev, *events = NULL;
    socklen_t s_len = 0;
    ssize_t bts = 0;

    s_len = sizeof(struct sockaddr_in);

    memset(msg, 0, MAX_MSG_SIZE);
    memset(&ev, 0, sizeof(struct epoll_event));

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

    strncpy((char*)msg, "Hello, bomberman server!", strlen("Hello, bomberman server!"));

    bts = sendto(sd, msg, strlen((char*)msg), 0, (struct sockaddr*)dst_addr, s_len);
    if (bts == -1) {
        perror("Client: sendto(dest_ip)");
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
    memset(dst_addr, 0, s_len);

    bts = recvfrom(sd, &p_id, sizeof(uint8_t), 0, (struct sockaddr*)dst_addr, &s_len);
    if (bts == -1) {
        perror("Client: recvfrom(dest_ip)");
        exit(EXIT_FAILURE);
    }

    s_len = sizeof(struct sockaddr_in);
    memset(dst_addr, 0, s_len);

    bts = recvfrom(sd, map, MAP_H * MAP_W, 0, (struct sockaddr*)dst_addr, &s_len);
    if (bts == -1) {
        perror("Client: recvfrom(dest_ip)");
        exit(EXIT_FAILURE);
    }

    strncpy((char*)msg, "Connection completed successfully", strlen("Connection completed successfully"));

    bts = sendto(sd, msg, strlen((char*)msg), 0, (struct sockaddr*)dst_addr, s_len);
    if (bts == -1) {
        perror("Client: sendto(dest_ip)");
        exit(EXIT_FAILURE);
    }

    c_info->sd = sd;
    c_info->dst_addr = *dst_addr;
    c_info->p_id = p_id;
    c_info->surface = surface;

    return p_id;
}

void *control_hndl(void* args) {
    int key = 0;
    uint8_t pressed_key = 0;
    connect_info_t *c_info = NULL;
    socklen_t s_len = 0;
    ssize_t bts = 0;

    s_len = sizeof(struct sockaddr_in);

    if (args == NULL) {
        perror("Client: args is NULL");
        exit(EXIT_FAILURE);
    }

    c_info = (connect_info_t *)args;

    do {
        key = getch();
        pressed_key = 0;

        switch(key) {
            case KEY_UP:
                pressed_key = KEY_U;
                break;

            case KEY_DOWN:
                pressed_key = KEY_D;
                break;

            case KEY_LEFT:
                pressed_key = KEY_L;
                break;

            case KEY_RIGHT:
                pressed_key = KEY_R;
                break;

            case KEY_SPACE_N:
                pressed_key = KEY_S;
                break;

            case KEY_ESC:
                pressed_key = KEY_E;
                break;
        }

        if (pressed_key != 0) {
            bts = sendto(c_info->sd, &pressed_key, sizeof(uint8_t), 0, (struct sockaddr*)&c_info->dst_addr, s_len);
            if (bts == -1) {
                perror("Client: sendto(dest_ip)");
                exit(EXIT_FAILURE);
            }
        }

    } while(1);
}

void *recv_hndl(void* args) {
    connect_info_t *c_info = NULL;
    ssize_t bts = 0;

    if (args == NULL) {
        perror("Client: args is NULL");
        exit(EXIT_FAILURE);
    }

    c_info = (connect_info_t *)args;

    do {
        bts = recvfrom(c_info->sd, map, MAP_H * MAP_W, 0, NULL, NULL);
        if (bts == -1) {
            perror("Client: recvfrom(dest_ip)");
            exit(EXIT_FAILURE);
        }

        print_map(c_info->surface, c_info->p_id);
    } while (1);
}
