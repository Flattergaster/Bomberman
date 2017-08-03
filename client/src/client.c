#include "../include/core.h"

int main(int argc, char **argv) {
    int sd = 0, *status = NULL;
    uint8_t p_id, dst_ip[IP_SIZE], src_ip[IP_SIZE];
    uint16_t dst_port = 0, src_port = 0;
    pthread_t ctl_hndl_tid = 0, recv_hndl_tid = 0;
    struct sockaddr_in dst_addr;
    struct hostent *dst_hst = NULL;
    connect_info_t c_info;
    surface_t *surface = NULL;

    parse_argvs(argc, argv, dst_ip, &dst_port, src_ip, &src_port);
    printf("[ARGS] DST: %s:%hu\n       SRC: %s:%hu\n", dst_ip, dst_port, src_ip, src_port);

    init_graph();
    init_surf(&surface);

    init_socket(&sd, &dst_addr, dst_ip, &dst_port);
    p_id = init_connect(sd, &dst_addr, surface,  &c_info);
    if (p_id == -1) {
        perror("Client: Incorrect player id");
        exit(EXIT_FAILURE);
    }

    /*c_info.sd = sd;
    c_info.dst_addr = dst_addr;
    c_info.p_id = p_id;
    c_info.surface = surface;

    printf("[INFO] Player ID: %hu\n", p_id);

    dst_hst = gethostbyaddr((char *)&dst_addr.sin_addr.s_addr, 4, AF_INET);
    printf("Connected to the server: %s:%hu (host name: %s)\n", inet_ntoa(dst_addr.sin_addr), ntohs(dst_addr.sin_port),
    ((dst_hst != NULL) ? dst_hst->h_name : ""));*/

    print_map(surface, p_id);

    pthread_create(&ctl_hndl_tid, NULL, control_hndl, (void *)&c_info);
    pthread_create(&recv_hndl_tid, NULL, recv_hndl, (void *)&c_info);

    pthread_join(ctl_hndl_tid, (void **)status);
    pthread_join(recv_hndl_tid, (void **)status);

    del_surf(&surface);
    close(sd);

    exit(EXIT_SUCCESS);
}
