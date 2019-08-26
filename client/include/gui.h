#ifndef GUI_H
#define GUI_H

#define _GNU_SORCE

#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "constants.h"

#define WND_W 51
#define WND_H 24
#define MAP_W 49
#define MAP_H 21
#define INFO_W 49
#define INFO_H 3

typedef struct _surface {
    WINDOW *p_wnd_map;
    WINDOW *wnd_map;
    WINDOW *p_wnd_info;
    WINDOW *wnd_info;
} surface_t;

void init_graph();
void init_surf(surface_t **surface);

void del_surf(surface_t **surface);
void print_map(surface_t *surface, unsigned char p_id);

#define PLAYER_U      L"\u263A"
#define ENEMY_U       L"\u2689"
#define BOMB_U        L"\u25CF"
#define FIRE_U        L"\u25CF"
#define ST_CELL_U     L"\u2588"
#define BR_CELL_U     L"\u2592"
#define EMPTY_CELL_U  L"\u2591"
#define RADIUS_BUFF_U L"\u254B"
#define POWER_BUFF_U  L"\u2620"
#define UNKNOWN_U 	  L"\u262D"
#define BM_PL_U       L"\u25C9"
#endif
