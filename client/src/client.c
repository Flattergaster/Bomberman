#include "../include/core.h"

int main(int argc, char **argv) {
    int sd = 0, i = 0, j = 0;
    unsigned char p_id, dst_ip[IP_SIZE], src_ip[IP_SIZE];
    unsigned short int dst_port = 0, src_port = 0;
    struct sockaddr_in dst_addr;
    struct hostent *dst_hst = NULL;

    parse_argvs(argc, argv, dst_ip, &dst_port, src_ip, &src_port);
    printf("[ARGS] DST: %s:%hu\n       SRC: %s:%hu\n", dst_ip, dst_port, src_ip, src_port);

    init_socket(&sd, &dst_addr, dst_ip, &dst_port);
    p_id = init_connect(sd, &dst_addr);
    if (p_id == -1) {
        perror("Client: Incorrect player id");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Player ID: %hu\n", p_id);

    dst_hst = gethostbyaddr((char *)&dst_addr.sin_addr.s_addr, 4, AF_INET);
    printf("Server: %s:%hu (hs: %s)\n", inet_ntoa(dst_addr.sin_addr), ntohs(dst_addr.sin_port),
    ((dst_hst != NULL) ? dst_hst->h_name : ""));

    printf("\n");
    for (i = 0; i < MAP_H; ++i) {
        for(j = 0; j < MAP_W; ++j)
            printf("%2d", map[i][j]);

        printf("\n");
    }

    close(sd);
    exit(EXIT_SUCCESS);
}
