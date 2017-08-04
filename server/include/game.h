#ifndef GAME_H
#define GAME_H

#include "../../shared/include/utils.h"

#define MAX_PLAYERS 10

#define ST_CELL      2
#define BR_CELL      1
#define EMPTY_CELL   0
#define BOMB         100
#define FIRE         101

#define P_MIN_ID     200
#define P_MAX_ID     210
#define MAX_BR_CELLS 150


void move(int index, int mov_x, int mov_y);
void move_player(int index, int key);
void set_player_pos(int index, int mov_x, int mov_y);
void apply_player_buff(int index, int b_type);
int generate_map();
void make_borders();
void gen_st_cells();
void gen_br_cells();

int kill_player(int index);

typedef struct _player {
    int p_id;
    int x;
    int y;
    int prev_x;
    int prev_y;
    int bomb_str;
    int bomb_pwr;
    int bomb_cur;
    int bomb_max;
    int sd;
    pthread_t tid_player;
    struct sockaddr_in end_point;
} player_t;

extern player_t players[];

unsigned char map[MAP_H][MAP_W];

/*pthread_mutex_t mutex_map = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_exit_player = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_player[MAX_PLAYERS];

uint8_t lowest_player_id = P_MIN_ID;*/

extern pthread_mutex_t mutex_map;
extern pthread_mutex_t mutex_exit_player;
extern pthread_mutex_t mutex_player[MAX_PLAYERS];

extern uint8_t lowest_player_id;

extern int exit_state;

#endif /* GAME_H*/