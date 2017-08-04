#include "../include/bomb_t.h"


void* bomb_thr(void* arg){
    int ind = 0;
    int i = 0, j = 0, prev_x = 0, prev_y = 0;
    int status = 0, max[4];

    ind = *((int*) arg);

    memset(max, 0, sizeof(max));

    prev_x = players[ind].prev_x;
    prev_y = players[ind].prev_y;

    if (map[prev_x][prev_y] == 0) {
        if (players[ind].bomb_cur < players[ind].bomb_max) {
            pthread_mutex_lock(&mutex_map);
            map[prev_x][prev_y] = BOMB;
            pthread_mutex_unlock(&mutex_map);

            pthread_mutex_lock(&mutex_player[ind]);
            players[ind].bomb_cur++;
            pthread_mutex_unlock(&mutex_player[ind]);
        }
    }
    else
        pthread_exit(NULL);

    status = sendto(
               players[ind].sd,
               map,
               MAP_H * MAP_W,
               0,
               &(players[ind].end_point),
               sizeof(players[ind].end_point));
    if (status < 0) {
        perror("sendto()");
        close(players[ind].sd);
        pthread_exit(NULL);
    }

    sleep(BOMB_TIMER_S);
    pthread_mutex_lock(&mutex_map);
    map[prev_x][prev_y] = EMPTY_CELL;
    pthread_mutex_unlock(&mutex_map);

    boom(max, ind, prev_x, prev_y);

    status = sendto(
               players[ind].sd,
               map,
               MAP_H * MAP_W,
               0,
               &(players[ind].end_point),
               sizeof(players[ind].end_point));
    if (status < 0) {
        perror("sendto()");
        close(players[ind].sd);
        pthread_exit(NULL);
    }

    usleep(CLEAR_TIMER_MS);

    clear_fire(max, prev_x, prev_y);

    status = sendto(
               players[ind].sd,
               map,
               MAP_H * MAP_W,
               0,
               &(players[ind].end_point),
               sizeof(players[ind].end_point));
    if (status < 0) {
        perror("sendto()");
        close(players[ind].sd);
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&mutex_player[ind]);
    players[ind].bomb_cur--;
    pthread_mutex_unlock(&mutex_player[ind]);

    pthread_exit(NULL);
}


void boom(int *max, int ind, int prev_x, int prev_y){
    pthread_mutex_lock(&mutex_map);

    max[0] = (players[ind].bomb_radius > MAP_H - prev_x)
            ? MAP_H - prev_x
            : players[ind].bomb_radius;
    boom_cell(1, 0, &max[0], prev_x, prev_y);

    max[1] = (players[ind].bomb_radius > MAP_W - prev_y)
            ? MAP_W - prev_y
            : players[ind].bomb_radius;
    boom_cell(0, 1, &max[1], prev_x, prev_y);

    max[2] = (players[ind].bomb_radius > prev_y)
            ? prev_y
            : players[ind].bomb_radius;
    boom_cell(0, -1, &max[2], prev_x, prev_y);

    max[3] = (players[ind].bomb_radius > prev_x)
            ? prev_x
            : players[ind].bomb_radius;
    boom_cell(-1, 0, &max[3], prev_x, prev_y);
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

void boom_cell(int cx, int cy, int *max, int prev_x, int prev_y){
    int flag = 0, i = 0, j = 0, x = 0, y = 0;

    for (i = 0; i < (*max); ++i) {
        x = prev_x + i*cx;
        y = prev_x + i*cy;

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
                for (j = 0; j < 10; ++j) {
                    if (players[i].p_id == map[x][y]) {
                        kill(i);
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
    int flag = 0, i = 0, j = 0, x = 0, y = 0;

    for (i = 0; i < max; ++i) {
        x = prev_x + i*cx;
        y = prev_x + i*cy;

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
