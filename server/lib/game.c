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
        do {
            br_x = rand() % MAP_H;
            br_y = rand() % MAP_W;
        } while (map[br_x][br_y] != EMPTY_CELL);

        map[br_x][br_y] = BR_CELL;
        br_cnt++;

    } while(br_cnt <  MAX_BR_CELLS);
}