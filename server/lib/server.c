#include "../include/server.h"

player_t players[MAX_PLAYERS];
unsigned char map[MAP_H][MAP_W];

int create_socket(struct sockaddr_in *addr, uint16_t port) {
    int sd = 0, i = 0;
    socklen_t size;

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd < 0) {
        perror("socket()");
        return -1;
    }

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(port);
    size = sizeof(*addr);

    if (bind(sd, addr, size) < 0) {
        if (errno == EADDRINUSE) {
            for (i = PORT + 1; i < MAX_PORT_VALUE; i++) {
                errno = 0;
                addr->sin_port = htons(i);
                
                if (bind(sd, addr, size) == 0) {
                    break;
                } 
            }
        } else {
            perror("bind()");
            return -1;
        }
    }

    return sd;
}

int create_player(int sd, struct sockaddr_in client_addr) {
    int index = 0;
    struct sockaddr_in server_addr;

    sd = create_socket(&server_addr, PORT);

    for (index = 0; index < MAX_PLAYERS; index++) {
        if (players[index].p_id == 0) {
            /*TODO after add shared constants*/
            players[index].p_id = rand() + 4;
            players[index].x = rand();
            players[index].y = rand();
            players[index].bomb_str = rand();
            players[index].bomb_pwr = rand();
            players[index].sd = sd;
            players[index].end_point = client_addr;
            return index;
        }
    }

    return -1;
}

int listener_new_clients(int sd) {
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    pthread_t *threads = NULL;
    int status = 0, sd_client = 0, index_client = 0;
    char *buffer = NULL;
    int *args_thread = NULL;
    socklen_t size_addr;

    size_addr = sizeof(client_addr);
    buffer = calloc(MAX_MESSAGE_SIZE, sizeof(char));
    threads = malloc(sizeof(pthread_t) * MAX_PLAYERS);
    args_thread = malloc(sizeof(int) * MAX_PLAYERS);

    printf("listen...\n");
    while (1) {
        status = recvfrom(
                    sd, 
                    buffer, 
                    MAX_MESSAGE_SIZE, 
                    0, 
                    &client_addr, 
                    &size_addr);
        if (status < 0) {
            perror("recvfrom()");
            return -1;
        }
        
        printf("Client connected\n");
        
        sd_client = create_socket(&server_addr, PORT);
        index_client = create_player(sd_client, client_addr);
        
        if (index_client < 0) {
            continue;
        }
        
        args_thread[index_client] = index_client;
        pthread_create(
                &threads[index_client],
                NULL,
                client_thread,
                &args_thread[index_client]);
    }

    close(sd);
    free(buffer);
    free(threads);

    return 0;
}

void *client_thread(void *args) {
    int index = *((int *) args);
    int status = 0;
    /*char *buffer = NULL;*/
    socklen_t size_addr = 0;

    size_addr = sizeof(players[index].end_point);
    
    status = accept_player(
                    players[index].sd,
                    &players[index].end_point,
                    &size_addr,
                    players[index].p_id);
    if (status < 0) {
        close(players[index].sd);
        pthread_exit(0);
    }
    
    close(players[index].sd);
    pthread_exit(0);
}

int accept_player(int sd, struct sockaddr_in *addr, 
                            socklen_t *addr_len, int p_id) {
    struct pollfd *poll_fd = NULL;
    int status = 0;
    char *buffer = NULL;
    
    poll_fd = malloc(sizeof(struct pollfd));
    buffer = malloc(sizeof(char) * MAX_MESSAGE_SIZE);

    poll_fd->fd = sd;
    poll_fd->events = POLLIN;
    
    status = sendto(
                sd,
                &p_id,
                sizeof(p_id),
                0,
                addr,
                *addr_len);
    if (status < 0) {
        perror("sendto()");
        free(poll_fd);
        free(buffer);
        close(sd);
        return -1;
    }
    
    status = sendto(
                sd,
                map,
                MAP_H * MAP_W,
                0,
                addr,
                *addr_len);
     if (status < 0) {
        perror("sendto()");
        free(poll_fd);
        free(buffer);
        close(sd);
        return -1;
    }
    
    printf("Send p_id & map to client\n");
    
    status = poll(poll_fd, 1, 10000);
    if (status < 0) {
        perror("poll()");
    }
    
    if (!status) {
        printf("Client did not respond in ten seconds.\n");
        free(poll_fd);
        free(buffer);
        close(sd);
        return -1;
    } else {
        status = recvfrom(
                    sd,
                    buffer,
                    MAX_MESSAGE_SIZE,
                    0,
                    addr,
                    addr_len);
        printf("Accept connection with client\n");
    }
    
    free(poll_fd);
    free(buffer);
    return 0;
}

int generate_map() {
    int i, j, k;
    
    memset(map, 0, MAP_H * MAP_W);
    
    for (j = 0; j < MAP_W; j++) {
        map[0][j] = 1;
        map[MAP_H - 1][j] = 1;
    }
    
     for (i = 0; i < MAP_H; i++) {
        map[i][0] = 1;
        map[i][MAP_W - 1] = 1;
    }
    
    for (i = 2; i < MAP_H - 1; i += 2) {
        for (j = 2; j < MAP_W - 1; j += 2) {
            for (k = 0; k < MAP_W; k += 2) {
                map[i][k] = 1;
            }
        }
    }
    /*
    for (i = 0; i < MAP_H; i++) {
        for (j = 0; j < MAP_W; j++) {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }*/
    return 0;
}
