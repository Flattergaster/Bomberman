#ifndef GAME_H
#define GAME_H

#include "../../shared/include/utils.h"

#define ST_CELL      2
#define BR_CELL      1
#define EMPTY_CELL   0
#define BOMB         100
#define FIRE         101
#define RADIUS_BUFF  150


#define MAX_PLAYERS  10
#define P_MIN_ID     200
#define P_MAX_ID     P_MIN_ID + MAX_PLAYERS - 1

#define MAX_BR_CELLS 150

#define MIN_BOMB_POWER  1
#define MAX_BOMB_POWER  4
#define MIN_BOMB_RADIUS 4
#define MAX_BOMB_RADIUS 10


void move(int index, int mov_x, int mov_y);
void move_player(int index, int key);
void set_player_pos(int index, int mov_x, int mov_y);
void apply_player_buff(int index, int b_type);
int generate_map();
void make_borders();
void gen_st_cells();
void gen_br_cells();
int kill_player(int index);
void update_lowest_free_id();
void find_random_cell(int *x, int *y, int c_type);
void *spawn_buffs(void *args);
void broadcast_map();


typedef struct _player {
    int p_id;
    int x;
    int y;
    int prev_x;
    int prev_y;
    int bomb_radius;
    int bomb_power;
    int bomb_cur;
    int bomb_max;
    int sd;
    pthread_t tid_player;
    struct sockaddr_in end_point;
} player_t;

extern player_t players[];

unsigned char map[MAP_H][MAP_W];

extern pthread_mutex_t mutex_map;
extern pthread_mutex_t mutex_player[MAX_PLAYERS];
extern pthread_mutex_t mutex_exit_player[MAX_PLAYERS];

extern uint8_t lowest_free_id;

extern int exit_state[MAX_PLAYERS];

#endif /* GAME_H*/