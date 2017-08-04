#ifndef BOMB_H
#define BOMB_H

void* bomb_thr(void* arg);
void boom(int ind, int prev_x, int prev_y);
void boom_cell(int ind, int cx, int cy, int *max, int prev_x, int prev_y);
void clear_cell(int ind, int cx, int cy, int max, int prev_x, int prev_y);

#endif
