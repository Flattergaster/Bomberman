#include "../include/server.h"
#include "../include/game.h"
#include "../include/bomb.h"

player_t players[MAX_PLAYERS];

pthread_mutex_t mutex_map = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_exit_player[MAX_PLAYERS];
pthread_mutex_t mutex_player[MAX_PLAYERS];

uint8_t lowest_free_id = P_MIN_ID;

int exit_state[MAX_PLAYERS];

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
            for (i = SERVER_PORT + 1; i < MAX_PORT_VALUE; i++) {
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

    sd = create_socket(&server_addr, SERVER_PORT);

    for (index = 0; index < MAX_PLAYERS; index++) {
        if (players[index].p_id == 0) {

            players[index].p_id = lowest_free_id;
            update_lowest_free_id();

            pthread_mutex_lock(&mutex_map);
        
            find_random_cell(&players[index].x, &players[index].y, EMPTY_CELL);
            map[players[index].x][players[index].y] = players[index].p_id;

            pthread_mutex_unlock(&mutex_map);

            players[index].bomb_radius = MIN_BOMB_RADIUS;
            players[index].bomb_power = MIN_BOMB_POWER;
            players[index].bomb_cur = 0;
            players[index].bomb_max = MAX_BOMBS;
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
    int *args_thread = NULL, i = 0;
    socklen_t size_addr;

    size_addr = sizeof(client_addr);
    buffer = calloc(MAX_MSG_SIZE, sizeof(char));
    threads = malloc(sizeof(pthread_t) * MAX_PLAYERS);
    args_thread = malloc(sizeof(int) * MAX_PLAYERS);
    
    for (i = 0; i < MAX_PLAYERS; i++) {
        mutex_player[i] = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
        mutex_exit_player[i] = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    }

    log_notice(stdout, "listen...\n");
    while (1) {
        status = recvfrom(
                    sd, 
                    buffer, 
                    MAX_MSG_SIZE, 
                    0, 
                    &client_addr, 
                    &size_addr);
        if (status < 0) {
            log_error(stdout, "recvfrom()");
            return -1;
        }
        
        log_notice(stdout, "Client connected\n");
        
        sd_client = create_socket(&server_addr, MAX_MSG_SIZE);
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
        
        players[index_client].tid_player = threads[index_client];
    }

    close(sd);
    free(buffer);
    free(threads);

    return 0;
}

void *client_thread(void *args) {
    int index = *((int *) args);
    int status = 0, breaking_packets = 0;
    uint8_t pressed_key = 0;
    socklen_t size_addr = 0;
    
    size_addr = sizeof(players[index].end_point);
    
    status = accept_player(
                    players[index].sd,
                    &players[index].end_point,
                    &size_addr,
                    players[index].p_id);
    if (status < 0) {
        log_error(stdout, "accept(): %s", strerror(errno));
       
        close(players[index].sd);
        pthread_exit((void *) -1);
    }
    
    while (1) {
        status = recvfrom(
                    players[index].sd,
                    &pressed_key,
                    sizeof(uint8_t),
                    0,
                    &players[index].end_point,
                    &size_addr);
        if (status < 0) {
            log_error(stdout, "recvfrom(key): %s", strerror(errno));
            if (breaking_packets < 3) {
                breaking_packets++;
                continue;
            } else {
                close(players[index].sd);
                pthread_exit((void *) -1);
            }
        }
        
        log_notice(stdout, "Client pressed key\n");
        
        pthread_mutex_lock(&mutex_map);
        status = do_action(index, pressed_key);
        pthread_mutex_unlock(&mutex_map);
        
        if (status == 0) {
            broadcast_map();
            
            pthread_mutex_lock(&mutex_exit_player[index]);
            if (exit_state[index] == 1) {
                exit_state[index] = 0;                
                break;
            }
            pthread_mutex_unlock(&mutex_exit_player[index]);

        }
    }
    
    close(players[index].sd);
    memset(&players[index], 0, sizeof(player_t));

    update_lowest_free_id();

    pthread_exit(0);
}

int accept_player(int sd, struct sockaddr_in *addr, 
                            socklen_t *addr_len, int p_id) {
    struct pollfd *poll_fd = NULL;
    int status = 0;
    char *buffer = NULL;
    
    poll_fd = malloc(sizeof(struct pollfd));
    buffer = malloc(sizeof(char) * MAX_MSG_SIZE);

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
                    MAX_MSG_SIZE,
                    0,
                    addr,
                    addr_len);
        printf("buffer: %s\n", buffer);
        printf("Accept connection with client\n");
    }
    
    free(poll_fd);
    free(buffer);
    return 0;
}

int do_action(int index, uint8_t key) {
    int i = 0, j = 0;
    pthread_t bomb_th;

    printf("Do action\n");
    switch (key) {
        case KEY_D:
        case KEY_U:
        case KEY_L:
        case KEY_R:
            move_player(index, key);
            break;
        case KEY_S:
            pthread_create(
                &bomb_th,
                NULL,
                bomb_thr,
                &index);
        
            break;
        case KEY_E:
            kill_player(index);
            break;
    }
    
    for (i = 0; i < MAP_H; i++) {
        for (j = 0; j < MAP_W; j++) {
            printf("%2d", map[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}

int start_buff_spawner() {
    int status;
    pthread_t buff_tid;

    status = pthread_create(&buff_tid, NULL, spawn_buffs, NULL);

    return status;
}