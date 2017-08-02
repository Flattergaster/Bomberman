#define _GNU_SORCE

#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncursesw/ncurses.h>
/*#include "../../shared/share.h"*/
/*#include "../../shared/include/utils.h"*/

#define WND_W 50
#define WND_H 24
#define MAP_W 48
#define MAP_H 20
#define INFO_W 48
#define INFO_H 3

typedef struct _surface {
    WINDOW *p_wnd_map;
    WINDOW *wnd_map;
    WINDOW *p_wnd_info;
    WINDOW *wnd_info;
} surface_t;

void init_graph();

void init_surf(surface_t **surface);

#define ST_CELL      2
#define BR_CELL      1
#define EMPTY_CELL   0
#define BOMB         100
#define P_MIN_ID     200
#define P_MAX_ID     210

#define PLAYER_U     L"\u00D6"
#define ENEMY_U      L"\u00D8"
#define BOMB_U       L"\u25CF"
#define ST_CELL_U    L"\u25A6"
#define BR_CELL_U    L"\u25A4"
#define EMPTY_CELL_U L"\u1234"
