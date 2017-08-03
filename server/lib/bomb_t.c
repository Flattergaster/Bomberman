#include "../include/server.h"

void* bomb_thr(void* arg){
    int ind = (int) arg;
    int i = 0, j = 0, prev_x = 0, prev_y = 0;
    char buffer[64];
    int max;
    prev_x = players[ind].prev_x;
    prev_y = players[ind].prev_y;
    memset(buffer, 0, sizeof(buffer));
    if (map[prev_x][prev_y] == 0) {
        if (players[ind].bomb_cur < players[ind].bomb_max) {
            /*mutex_lock*/
            map[prev_x][prev_y] = BOMB;
            /*mutex_unlock*/

            /*mutex_bomb_lock*/
            players[ind].bomb_cur++;
            /*mutex_bomb_lock*/
        }
    }
    status = sendto(
               players[ind].sd,
               map,
               MAP_H * MAP_W,
               0,
               &(players[ind].end_point),
               sizeof(players[ind].end_point));
    if (status < 0) {
        perror("sendto()");
        free(poll_fd);
        free(buffer);
        close(sd);
        return -1;
    }
    sleep(3);
    boom(ind, prev_x, prev_y);
}


void boom(int ind, int prev_x, int prev_y){
    int max[4];

    max[0] = (players[ind].bomb_str > MAP_H - prev_x)
            ? MAP_H - prev_x
            : players[ind].bomb_str;
    /*mutex_lock*/
    boom_cell(ind, 1, 0, &max[0], prev_x, prev_y);
    /*mutex_unlock*/

    max[1] = (players[ind].bomb_str > MAP_W - prev_y)
            ? MAP_W - prev_y
            : players[ind].bomb_str;
    /*mutex_lock*/
    boom_cell(ind, 0, 1, &max[1], prev_x, rpev_y);
    /*mutex_unlock*/

    max[2] = (players[ind].bomb_str > prev_y)
            ? prev_y
            : players[ind].bomb_str;
    /*mutex_lock*/
    boom_cell(ind, 0, -1, &max[2], prev_x, rpev_y);
    /*mutex_unlock*/

    /*mutex_lock*/
    max[3] = (players[ind].bomb_str > prev_x)
            ? prev_x
            : players[ind].bomb_str;
    boom_cell(ind, -1, 0, &max[3], prev_x, prev_y);
    /*mutex_unlock*/

    /*mutex_lock*/
    clear_cell(ind, 1, 0, max[0], prev_x, prev_y);
    /*mutex_unlock*/

    /*mutex_lock*/
    clear_cell(ind, 0, 1, max[1], prev_x, prev_y);
    /*mutex_unlock*/

    /*mutex_lock*/
    clear_cell(ind, 0, -1, max[2], prev_x, prev_y);
    /*mutex_unlock*/

    /*mutex_lock*/
    clear_cell(ind, -1, 0, max[3], prev_x, prev_y);
    /*mutex_unlock*/
}

void boom_cell(int ind, int cx, int cy, int *max, int prev_x, int prev_y){
    int flag = 0m i = 0, j = 0, x = 0, y = 0;
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
            case P_MIN_ID ... P_MAX_ID:
                for (j = 0; j < 10; ++j) {
                    if (players[i].p_id == map[x][y]) {
                        kill(i);
                        break;
                    }
                }
                map[x][y] = FIRE;
                break;
            case EMPTY_CELL:
                map[x][y] = FIRE;
                break;
            case FIRE:
                map[x][y]++;
                break;
            default:
                break;
        }
        if(flag == 1) break;
    }
}

void clear_cell(int ind, int cs, int cy, int max, int prev_x, int prev_y){
    int flag = 0m i = 0, j = 0, x = 0, y = 0;
    for (i = 0; i < max; ++i) {
        x = prev_x + i*cx;
        y = prev_x + i*cy;
        switch (map[x][y]) {
            case (FIRE + 1) ... (FIRE + 10):
                map[x][y]--;
                break;
            case FIRE:
                map[x][y] = 0;
                break;
            default:
                break;
        }
        if(flag == 1) break;
    }
}
