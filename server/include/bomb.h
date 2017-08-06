#ifndef BOMB_H
#define BOMB_H

#include "../include/game.h"
// #include "../include/server.h"

#define BOMB_TIMER_S 2
#define CLEAR_TIMER_MS 500000

#define MAX_BOMBS 4

void* bomb_thr(void* arg);
void boom(int *max, int ind, int prev_x, int prev_y, int *dead_players);
void clear_fire(int *max, int prev_x, int prev_y);
void boom_cell(int cx, int cy, int *max, int prev_x, int prev_y, int *dead_players);
void clear_cell(int cx, int cy, int max, int prev_x, int prev_y);
int plant_bomb(int ind, int x, int y);

#endif
