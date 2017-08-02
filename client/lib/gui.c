#include "../include/gui.h"
unsigned char map[MAP_H][MAP_W];


void init_graph() {
    WINDOW *rtn = NULL;

    rtn = initscr();
    if (rtn == NULL) {
        perror("Client: initscr error");
        exit(EXIT_FAILURE);
    }
    setlocale(LC_ALL, 0);
    /*noecho();*/
    curs_set(0);
    /*keypad(stdscr, true);*/
    /*cbreak();*/
    start_color();
    refresh();
    /*LOG - init_graph*/
}

void init_surf(surface_t **surface) {
    (*surface)->p_wnd_map = newwin(MAP_H, MAP_W, INFO_H + 1, 1);
    box((*surface)->p_wnd_map, 0, 0);
    (*surface)->wnd_map = derwin((*surface)->p_wnd_map,
                                    MAP_H - 2, MAP_W - 2, 1, 1);

    (*surface)->p_wnd_info = newwin(MAP_H, MAP_W, INFO_H + 1, 1);
    box((*surface)->p_wnd_info, 0, 0);
    (*surface)->wnd_info = derwin((*surface)->p_wnd_info,
                                    MAP_H - 2, MAP_W - 2, 1, 1);

    wrefresh((*surface)->p_wnd_map);
    wrefresh((*surface)->p_wnd_info);
    /*LOG - init_win*/
}

void del_surf(surface_t **surface){
    delwin((*surface)->p_wnd_map);
    delwin((*surface)->p_wnd_info);
    delwin((*surface)->wnd_map);
    delwin((*surface)->wnd_info);
    endwin();
}

void print_map(surface_t *surface, unsigned char p_id){
    int i = 0, j = 0;
    wchar_t wstr[] = {0x534D, L'\0'};
    for (i = 0; i < MAP_H; ++i) {
        for(j = 0; j < MAP_W; ++j) {
            switch (map[i][j]) {
                case EMPTY_CELL:
                    waddwstr(surface->wnd_map, EMPTY_CELL_U);
                    break;
                case BR_CELL:
                    waddwstr(surface->wnd_map, BR_CELL_U);
                    break;
                case ST_CELL:
                    waddwstr(surface->wnd_map, ST_CELL_U);
                    break;
                case P_MIN_ID ... P_MAX_ID:
                    if (map[i][j] == p_id)
                        waddwstr(surface->wnd_map, PLAYER_U);
                    else
                        waddwstr(surface->wnd_map, ENEMY_U);
                    break;
                case BOMB:
                        waddwstr(surface->wnd_map, wstr/*BOMB_U*/);
                    break;
                default:
                    break;
            }
        }
    }
    wrefresh(surface->wnd_map);
}


int main(){
    init_graph();
    memset(map, 100, MAP_H * MAP_W);
    surface_t *surface;
    map[10][10] = 201;
    map[15][15] = 100;
    surface = malloc(sizeof(surface_t));
    init_surf(&surface);
    print_map(surface, 201);
    getchar();
    del_surf(&surface);
    return 0;
}
