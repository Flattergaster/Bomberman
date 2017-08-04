#ifndef BOMB_H
#define BOMB_H

#define BOMB_TIMER_S 3
#define CLEAR_TIMER_MS 500000

void* bomb_thr(void* arg);
void boom(int *max, int ind, int prev_x, int prev_y);
void clear_fire(int *max, int prev_x, int prev_y);
void boom_cell(int cx, int cy, int *max, int prev_x, int prev_y);
void clear_cell(int cx, int cy, int max, int prev_x, int prev_y);

#endif
