#include "../include/game.h"

int generate_map() {
    memset(map, 0, MAP_H * MAP_W);

    make_borders();
    gen_st_cells();
    gen_br_cells();

    return 0;
}

void make_borders() {
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
    int i, j, k;

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
        find_random_cell(&br_x, &br_y, EMPTY_CELL);

        map[br_x][br_y] = BR_CELL;
        br_cnt++;

    } while(br_cnt <  MAX_BR_CELLS);
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
        case POWER_BUFF:
            set_player_pos(index, mov_x, mov_y);
            apply_player_buff(index, POWER_BUFF);
            break;
        case RADIUS_BUFF:
            set_player_pos(index, mov_x, mov_y);
            apply_player_buff(index, RADIUS_BUFF);
            break;
        default:
            break;
    }
}

void set_player_pos(int index, int mov_x, int mov_y) {
    int cur_x = 0, cur_y = 0;
    
    cur_x = players[index].x;
    cur_y = players[index].y;

    if(map[cur_x][cur_y] >= BM_PL_MIN && map[cur_x][cur_y] <= BM_PL_MAX )
        map[cur_x][cur_y] = BOMB;
    else
        map[cur_x][cur_y] = EMPTY_CELL;

    map[mov_x][mov_y] = players[index].p_id;

    players[index].x = mov_x;
    players[index].y = mov_y;
    players[index].prev_x = cur_x;
    players[index].prev_y = cur_y;
}

void apply_player_buff(int index, int b_type) {
    int x, y;

    x = players[index].x;
    y = players[index].y;
    
    switch (b_type) {
        case POWER_BUFF:
            if (players[index].bomb_power < MAX_BOMB_POWER) {
                players[index].bomb_power += 1;
            }
            break;
        case RADIUS_BUFF:
            if (players[index].bomb_radius < MAX_BOMB_RADIUS) {
                players[index].bomb_radius += 1;
            }
            break;
        default:
            break;
    }

}

int kill_player(int index) {
    int status = 0;
    if (players[index].p_id != 0) {        
        status = sendto(
                    players[index].sd,
                    "Game Over!",
                    strlen("Game Over!"),
                    0,
                    (struct sockaddr *)&players[index].end_point,
                    sizeof(players[index].end_point));
        if (status < 0) {
            log_error(stdout, "sendto(): %s", strerror(errno));
            return -1;
        }
        
        pthread_mutex_lock(&mutex_exit_player[index]);
        exit_state[index] = 1;
        pthread_mutex_unlock(&mutex_exit_player[index]);
        return 0;
    }
    return -1;
}

void update_lowest_free_id() {
    int i, j, flag;
    for (i = P_MIN_ID; i <= P_MAX_ID; i++) {
        flag = 0;
        for(j = 0; j < MAX_PLAYERS; j++) {
            if (players[j].p_id == i) {
                flag = 1;
                break;
            }
        }
        printf("%d ", flag);
        if (flag == 0) {
            lowest_free_id = i;
            return;
        }
    }
}

void find_random_cell(int *x, int *y, int c_type) {
    do {
        *x = rand() % MAP_H;
        *y = rand() % MAP_W;                
    } while (map[*x][*y] != c_type);
}

void *spawn_buffs(void *args) {
    int x = 0, y = 0, buff = EMPTY_CELL;//, buff_cnt = 0;
    int chance;

    while (1) {
        srand(time(NULL));
        chance = rand() % 101;
    
        // if (buff_cnt < MAX_BUFFS) {
            
        if (chance < POWER_B_CHANCE)
            buff = POWER_BUFF;
        else if (chance < RADIUS_B_CHANCE)
            buff = RADIUS_BUFF;

        pthread_mutex_lock(&mutex_map);
        find_random_cell(&x, &y, EMPTY_CELL);
        map[x][y] = buff;
        pthread_mutex_unlock(&mutex_map);
        
            // buff_cnt++;
        // }
        if (buff != EMPTY_CELL)
            broadcast_map("spawn buffs");
    
        sleep(15);
        buff = EMPTY_CELL;
    }
}

void broadcast_map(const char *action) {
    int i = 0, status = 0;
    log_notice(stdout, "Broadcast map: %s\n", action);
    for (i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].p_id != 0) {
            status = sendto(
                        players[i].sd,
                        map,
                        MAP_H * MAP_W,
                        0,
                        (struct sockaddr *)&players[i].end_point,
                        sizeof(players[i].end_point));
             if (status < 0) {
                log_error(stdout, "sendto(map): %s", strerror(errno));
            }
        }
    }
}