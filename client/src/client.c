#include "../include/core.h"

int main(int argc, char **argv) {
    int sd = 0, *status = NULL;
    uint8_t dst_ip[IP_SIZE], src_ip[IP_SIZE];
    uint16_t dst_port = 0, src_port = 0;
    pthread_t ctl_hndl_tid = 0, recv_hndl_tid = 0;
    struct sigaction sig_act;
    struct sockaddr_in dst_addr;
    connect_info_t c_info;
    surface_t *surface = NULL;

    parse_argvs(argc, argv, dst_ip, &dst_port, src_ip, &src_port);
    log_notice(stdout, "args dst: %s:%hu src: %s:%hu", dst_ip, dst_port, src_ip, src_port);

    init_signal(&sig_act);

    init_graph();
    init_surf(&surface);

    init_socket(&sd, &dst_addr, dst_ip, &dst_port);
    init_connect(sd, &dst_addr, surface,  &c_info);

    log_notice(stdout, "connected to the server: %s:%hu", dst_ip, dst_port);
    log_notice(stdout, "player ID: %hu", c_info.p_id);

    pthread_create(&ctl_hndl_tid, NULL, control_hndl, (void *)&c_info);
    pthread_create(&recv_hndl_tid, NULL, recv_hndl, (void *)&c_info);

    pthread_join(ctl_hndl_tid, (void **)status);
    /*pthread_join(recv_hndl_tid, (void **)status);*/

    del_surf(&surface);
    close(sd);

    exit(EXIT_SUCCESS);
}
