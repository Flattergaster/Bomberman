#include "../include/core.h"

int main(int argc, char **argv) {
    int sd = 0, i = 0, j = 0, *status = NULL;
    uint8_t p_id, dst_ip[IP_SIZE], src_ip[IP_SIZE];
    uint16_t dst_port = 0, src_port = 0;
    pthread_t ctl_hndl_tid;
    struct sockaddr_in dst_addr;
    struct hostent *dst_hst = NULL;
    connect_info_t c_info;

    parse_argvs(argc, argv, dst_ip, &dst_port, src_ip, &src_port);
    printf("[ARGS] DST: %s:%hu\n       SRC: %s:%hu\n", dst_ip, dst_port, src_ip, src_port);

    init_socket(&sd, &dst_addr, dst_ip, &dst_port);
    p_id = init_connect(sd, &dst_addr);
    if (p_id == -1) {
        perror("Client: Incorrect player id");
        exit(EXIT_FAILURE);
    }

    c_info.sd = sd;
    c_info.dst_addr = dst_addr;

    printf("[INFO] Player ID: %hu\n", p_id);

    dst_hst = gethostbyaddr((char *)&dst_addr.sin_addr.s_addr, 4, AF_INET);
    printf("Connected to the server: %s:%hu (host name: %s)\n", inet_ntoa(dst_addr.sin_addr), ntohs(dst_addr.sin_port),
    ((dst_hst != NULL) ? dst_hst->h_name : ""));

    printf("\n");
    for (i = 0; i < MAP_H; ++i) {
        for(j = 0; j < MAP_W; ++j)
            printf("%2d", map[i][j]);

        printf("\n");
    }
    printf("\n");

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
        if (initscr() == NULL) {
                perror("initscr");
                exit(EXIT_FAILURE);
        }

        noecho();
        curs_set(0);
        keypad(stdscr, true);
        cbreak();
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

    pthread_create(&ctl_hndl_tid, NULL, control_hndl, (void *)&c_info);
    pthread_join(ctl_hndl_tid, (void **)status);

    close(sd);
    exit(EXIT_SUCCESS);
}
