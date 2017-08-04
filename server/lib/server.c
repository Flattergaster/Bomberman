#include "../include/server.h"

player_t players[MAX_PLAYERS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

uint8_t lowest_player_id = P_MIN_ID;

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

    sd = create_socket(&server_addr, SERVER_PORT);

    for (index = 0; index < MAX_PLAYERS; index++) {
        if (players[index].p_id == 0) {
            /*TODO after add shared constants*/
            players[index].p_id = lowest_player_id;
            lowest_player_id++;

            do {
                players[index].x = rand() % MAP_H;
                players[index].y = rand() % MAP_W;                
            } while (map[players[index].x][players[index].y] == ST_CELL);

            map[players[index].x][players[index].y] = players[index].p_id;
            /*TODO after add shared constants*/
            players[index].bomb_str = 1;
            players[index].bomb_pwr = 3;
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
        
        players[index_client].tid_player = threads[index_client];
    }

    close(sd);
    free(buffer);
    free(threads);

    return 0;
}

void *client_thread(void *args) {
    int index = *((int *) args);
    int status = 0;
    uint8_t pressed_key = 0;
    socklen_t size_addr = 0;
    
    status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (status != 0) {
        perror("pthread_setcancelstate()");
        close(players[index].sd);
        pthread_exit(0);
    }
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
    
    while (1) {
        recvfrom(
            players[index].sd,
            &pressed_key,
            sizeof(uint8_t),
            0,
            &players[index].end_point,
            &size_addr);
        printf("Client pressed key\n");
        pthread_mutex_lock(&mutex);
        do_action(index, pressed_key);
        pthread_mutex_unlock(&mutex);
        printf("Send action on the pressed key\n");
        sendto(
            players[index].sd,
            map,
            MAP_H * MAP_W,
            0,
            &players[index].end_point,
            size_addr);
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
        printf("buffer: %s\n", buffer);
        printf("Accept connection with client\n");
    }
    
    free(poll_fd);
    free(buffer);
    return 0;
}

int do_action(int index, uint8_t key) {
    int i = 0, j = 0;
    
    printf("Do action\n");
    switch (key) {
        case KEY_D:
        case KEY_U:
        case KEY_L:
        case KEY_R:
            move_player(index, key);
        case KEY_S:
            /*TODO boom thread*/
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

void move_player(int index, int key) {
    int cur_x = 0, cur_y = 0;
    int mov_x = 0, mov_y = 0;
    
    cur_x = players[index].x;
    cur_y = players[index].y;

    switch (key) {
        case KEY_D:
            mov_x = cur_x + 1;
            mov_y = cur_y;
            break;
        case KEY_U:
            mov_x = cur_x - 1;
            mov_y = cur_y;
            break;
        case KEY_L:
            mov_x = cur_x;
            mov_y = cur_y - 1;
            break;
        case KEY_R:
            mov_x = cur_x;
            mov_y = cur_y + 1;
            break;
    }

    move(index, mov_x, mov_y);
}

void move(int index, int mov_x, int mov_y) {

    switch (map[mov_x][mov_y]) {
        case EMPTY_CELL:
            set_player_pos(index, mov_x, mov_y);
            break;
        /*case POWER_BUFF:
            set_player_pos(index, mov_x, mov_y);
            apply_player_buff(index, POWER_BUFF);
            break;
        case STRENGTH_BUFF:
            set_player_pos(index, mov_x, mov_y);
            apply_player_buff(index, STRENGTH_BUFF);
            break;*/
        default:
            break;
    }
}

void set_player_pos(int index, int mov_x, int mov_y) {
    int cur_x = 0, cur_y = 0;
    
    cur_x = players[index].x;
    cur_y = players[index].y;

    map[mov_x][mov_y] = players[index].p_id;
    map[cur_x][cur_y] = EMPTY_CELL; 
    
    players[index].x = mov_x;
    players[index].y = mov_y;
    players[index].prev_x = cur_x;
    players[index].prev_y = cur_y;
}

void apply_player_buff(int index, int b_type) {
    /*
    switch (b_type) {
        case POWER_BUFF:
            players[index].bomb_pwr += 1;
            break;
        case STRENGTH_BUFF:
            players[index].bomb_str += 1;
            break;
        default:
            break;
    }
    */
}

int kill_player(int index) {
    int status = 0;
    if (players[index].p_id != 0) {
        pthread_cancel(players[index].tid_player);
        
        status = sendto(
                    players[index].sd,
                    "Game Over!",
                    strlen("Game Over!"),
                    0,
                    &players[index].end_point,
                    sizeof(players[index].end_point));
        if (status < 0) {
            perror("sendto()");
            return -1;
        }
        
        close(players[index].sd);
        memset(&players[index], 0, sizeof(player_t));
        return 0;
    }
    return -1;
}

int generate_map() {
    memset(map, 0, MAP_H * MAP_W);

    make_borders();
    gen_st_cells();
    gen_br_cells();

    return 0;
}

void make_borders()
{
    int i, j;
    for (j = 0; j < MAP_W; j++) {
        map[0][j] = ST_CELL;
        map[MAP_H - 1][j] = ST_CELL;
    }
    
     for (i = 0; i < MAP_H; i++) {
        map[i][0] = ST_CELL;
        map[i][MAP_W - 1] = ST_CELL;
    }
}

void gen_st_cells() {
    int i, j;

    for (i = 2; i < MAP_H - 1; i += 2) {
        for (j = 2; j < MAP_W - 1; j += 2) {
            for (k = 0; k < MAP_W; k += 2) {
                map[i][k] = ST_CELL;
            }
        }
    }
}

void gen_br_cells() {
    int br_cnt = 0, br_x = 0, br_y = 0;

    do {
        do {
            br_x = rand() % MAP_H;
            br_y = rand() % MAP_W;
        } while (map[br_x][br_y] != EMPTY_CELL);

        map[br_x][br_y] = BR_CELL;
        br_cnt++;

    } while(br_cnt <  MAX_BR_CELLS);
}