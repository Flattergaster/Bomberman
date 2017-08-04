#ifndef PLAYER_H
#define PLAYER_H

#define ST_CELL      2
#define BR_CELL      1
#define EMPTY_CELL   0
#define P_MIN_ID     200
#define P_MAX_ID     210
#define MAX_BR_CELLS 150

void move(int index, int mov_x, int mov_y);
void move_player(int index, int key);
void set_player_pos(int index, int mov_x, int mov_y);
void apply_player_buff(int index, int b_type);
void make_borders();
void gen_st_cells();
void gen_br_cells();

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

#endif /* PLAYER_H*/