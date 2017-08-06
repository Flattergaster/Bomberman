#include "../include/bomb.h"

void* bomb_thr(void* arg){
    int ind = 0;
    int i = 0, status = 0, prev_x = 0, prev_y = 0;
    int  max[4];
    int dead_players[MAX_PLAYERS];
    ind = *((int*) arg);

    memset(dead_players, 0, MAX_PLAYERS * sizeof(int));
    memset(max, 0, sizeof(max));

    prev_x = players[ind].prev_x;
    prev_y = players[ind].prev_y;

    status = plant_bomb(ind, prev_x, prev_y);

    if (status < 0) {
        pthread_exit(NULL);
    }

    broadcast_map();

    sleep(BOMB_TIMER_S);
    
    pthread_mutex_lock(&mutex_map);
    map[prev_x][prev_y] = EMPTY_CELL;
    pthread_mutex_unlock(&mutex_map);

    boom(max, ind, prev_x, prev_y, dead_players);

    broadcast_map();

    usleep(CLEAR_TIMER_MS);

    clear_fire(max, prev_x, prev_y);

    broadcast_map();

    pthread_mutex_lock(&mutex_player[ind]);
    players[ind].bomb_cur--;
    pthread_mutex_unlock(&mutex_player[ind]);

    for (i = 0; i < MAX_PLAYERS; ++i) {
        if (dead_players[i] == 1) {
            kill_player(i);
        }
    }

    pthread_exit(NULL);
}

int plant_bomb(int ind, int x, int y) {
    int status = 0;

    pthread_mutex_lock(&mutex_map);
    pthread_mutex_lock(&mutex_player[ind]);

    if (map[x][y] == 0 
        && players[ind].bomb_cur < players[ind].bomb_max) {

            map[x][y] = BOMB;
            players[ind].bomb_cur++;
        }
    else
       status = -1;

    pthread_mutex_unlock(&mutex_player[ind]);
    pthread_mutex_unlock(&mutex_map);
    
    return status;
}


void boom(int *max, int ind, int prev_x, int prev_y, int *dead_players){
    pthread_mutex_lock(&mutex_map);

    for(int i = 0; i < 4; ++i) {
        max[i] = players[ind].bomb_radius;
    }

    // max[0] = (players[ind].bomb_radius > MAP_H - prev_x)
            // ? MAP_H - prev_x
            // : players[ind].bomb_radius;
    boom_cell(1, 0, &max[0], prev_x, prev_y, dead_players);

    // max[1] = (players[ind].bomb_radius > MAP_W - prev_y)
    //         ? MAP_W - prev_y
    //         : players[ind].bomb_radius;
    boom_cell(0, 1, &max[1], prev_x, prev_y, dead_players);

    // max[2] = (players[ind].bomb_radius > prev_y)
    //         ? prev_y
    //         : players[ind].bomb_radius;
    boom_cell(0, -1, &max[2], prev_x, prev_y, dead_players);

    // max[3] = (players[ind].bomb_radius > prev_x)
    //         ? prev_x
    //         : players[ind].bomb_radius;
    boom_cell(-1, 0, &max[3], prev_x, prev_y, dead_players);
    pthread_mutex_unlock(&mutex_map);

}

void clear_fire(int *max, int prev_x, int prev_y){
    pthread_mutex_lock(&mutex_map);

    clear_cell(1, 0, max[0], prev_x, prev_y);
    clear_cell(0, 1, max[1], prev_x, prev_y);
    clear_cell(0, -1, max[2], prev_x, prev_y);
    clear_cell(-1, 0, max[3], prev_x, prev_y);

    pthread_mutex_unlock(&mutex_map);
}

void boom_cell(int cx, int cy, int *max, int prev_x, int prev_y, int *dead_players){
    int flag = 0, i = 0, j = 0, x = 0, y = 0;

    for (i = 0; i < (*max); ++i) {
        x = prev_x + i*cx;
        y = prev_y + i*cy;

        switch (map[x][y]) {
            case ST_CELL:
                flag = 1;
                (*max) = i;/* ??? */
                break;
            case BR_CELL:
                map[x][y] = FIRE;
                flag = 1;
                (*max) = i + 1;/* ??? */
                break;
            case EMPTY_CELL:
                map[x][y] = FIRE;
                break;
            case P_MIN_ID ... P_MAX_ID:
                for (j = 0; j < MAX_PLAYERS; ++j) {
                    if (players[j].p_id == map[x][y]) {
                        dead_players[j] = 1;
                        break;
                    }
                }
                map[x][y] = FIRE;
                break;
            case FIRE:
                if(x == prev_x && y == prev_y)
                    break;
                else
                   map[x][y]++;
                break;
            default:
                break;
        }
        if(flag == 1)
            break;
    }
}

void clear_cell(int cx, int cy, int max, int prev_x, int prev_y){
    int flag = 0, i = 0, x = 0, y = 0;

    for (i = 0; i < max; ++i) {
        x = prev_x + i*cx;
        y = prev_y + i*cy;

        switch (map[x][y]) {
            case (FIRE + 1) ... (FIRE + 10):
                map[x][y]--;
                break;
            case FIRE:
                map[x][y] = EMPTY_CELL;
                break;
            default:
                break;
        }
        if(flag == 1) break;
    }
}
