#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "mapa.h"
#include "state.h"

#define TRAP_PERCENTAGE 10
#define TRAP_COLOR_2 8
#define WALL_COLOR 7
#define HEAL_OFF 12
#define BULLET_OFF 16
#define HOUSE_COLOR 30


void draw_map(MAPA* map, int *map_visibility) {
    //#region MAP_GENERATOR
    srand(time(NULL));
    start_color();
    init_pair(TRAP_COLOR_2, COLOR_BLACK, COLOR_BLACK);
    if(*map_visibility){
     init_pair(WALL_COLOR, COLOR_CYAN, COLOR_BLUE);
    }else init_pair(WALL_COLOR, COLOR_BLACK, COLOR_BLACK);
    init_pair(HEAL_OFF,COLOR_BLACK,COLOR_BLACK);
    init_pair (BULLET_OFF, COLOR_BLACK,COLOR_BLACK);
    init_pair(HOUSE_COLOR,COLOR_BLACK,COLOR_BLACK);

    int count1 = 0;
    
    attron(COLOR_PAIR(WALL_COLOR));
    for (int i = 0; i < map->x; i++) {
        map->matrix[i][0] = '#'; 
        map->matrix[i][map->y - 2] = '#';
        mvaddch(0, i, map->matrix[i][0]);
        mvaddch(map->y - 2, i, map->matrix[i][map->y - 2]);
        count1++;
    }
    for (int i = 0; i < map->y - 1; i++) {
        map->matrix[0][i] = '#'; 
        map->matrix[map->x - 1][i] = '#'; 
        mvaddch(i, 0, map->matrix[0][i]);
        mvaddch(i, map->x - 1, map->matrix[map->x - 1][i]);
        count1++;
    }
   
    for (int i = 1; i < map->x - 1; i++) {
        map->matrix[i][map->y - 1] = ' '; 
        mvaddch(map->y - 1, i, map->matrix[i][map->y - 1]);
    }
    int casas_totais = ((map->x) * (map->y)) - count1;
    int heal_percentage = casas_totais * 0.01;
    int heal_count = 0;
    int trap_count = 0;
    int trap_percentage = casas_totais * 0.01;
    int bullet_count = 0;
    int bullet_percentage = casas_totais * 0.01;

    
    for (int i = 0; i < casas_totais * 0.4; i++) {
        int x1 = 2 + rand() % (map->x - 2); 
        int y1 = 2 + rand() % (map->y - 2);
        map->matrix[x1][y1] = '#';
        mvaddch(y1, x1, map->matrix[x1][y1]);
    }

    for (int i = 0; i < 3; i++) { 
        for (int y = 1; y < (map->y-1) - 1; y++) {
            for (int x = 1; x < (map->x) - 1; x++) {
                int count = 0;
                for (int yy = y - 1; yy <= y + 1; yy++) {
                    for (int xx = x - 1; xx <= x + 1; xx++) {
                        if (map->matrix[xx][yy] == '#') {
                            count++;
                        }
                    }
                }
                if (map->matrix[x][y] == '#') {
                    if (count < 3) {
                         attroff(COLOR_PAIR(WALL_COLOR));
                         attron (COLOR_PAIR(HOUSE_COLOR));
                        map->matrix[x][y] = ' ';
                        mvaddch(y,x,map->matrix[x][y]);
                         attroff(COLOR_PAIR(HOUSE_COLOR));
                         attron (COLOR_PAIR(WALL_COLOR));
                    }
                } else {
                    if (count > 4) {
                        map->matrix[x][y] = '#';
                        mvaddch(y,x,map->matrix[x][y]);
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < 4; i++) { 
        for (int y = 1; y < (map->y-1) - 1; y++) {
            for (int x = 1; x < (map->x) - 1; x++) {
                int count = 0;
                for (int iy = y - 1; iy <= y + 1; iy++) {
                    for (int ix = x - 1; ix <= x + 1; ix++) {
                        if (map->matrix[ix][iy] == '#' && (iy != y || ix != x)) {
                            count++;
                        }
                    }
                }
                if (map->matrix[x][y] == '#') {
                    if (count < 3) {
                         attroff(COLOR_PAIR(WALL_COLOR));
                         attron (COLOR_PAIR(HOUSE_COLOR));
                        map->matrix[x][y] = ' ';
                        mvaddch(y,x,map->matrix[x][y]);
                         attroff(COLOR_PAIR(HOUSE_COLOR));
                         attron (COLOR_PAIR(WALL_COLOR));
                    }
                }
                if (map->matrix[x][y]== ' ') {
                    if (count > 4) {
                        map->matrix[x][y] = '#';
                        mvaddch(y,x,map->matrix[x][y]);
                    }
                }
            }
        }
    }


 for (int i = 1; i < map->x - 1; i++) {
    for (int j = 1; j < map->y - 1; j++) {
        if (map->matrix[i][j] == '#') {
            int count2 = 0;
            if (map->matrix[i - 1][j] == '#') count2++; 
            if (map->matrix[i + 1][j] == '#') count2++; 
            if (map->matrix[i][j - 1] == '#') count2++; 
            if (map->matrix[i][j + 1] == '#') count2++; 

            if (count2 == 0) {
                attroff(COLOR_PAIR(WALL_COLOR));
                attron (COLOR_PAIR(HOUSE_COLOR));
                map->matrix[i][j] = ' '; 
                mvaddch(j, i, map->matrix[i][j]);
                attroff(COLOR_PAIR(HOUSE_COLOR));
                attron(COLOR_PAIR(WALL_COLOR));
            }
        }
    }
}
attroff(COLOR_PAIR(WALL_COLOR));
//#endregion

// #region MAP_ITEMS 
attron (COLOR_PAIR(TRAP_COLOR_2));
while  (trap_count < trap_percentage) {
 int x1 = rand() % (map->x - 2) + 1;
 int y1 = rand() % (map->y - 1);
 if (map->matrix [x1] [y1] != '#') {
  map->matrix[x1][y1] = 'x';
  mvaddch(y1, x1, map->matrix[x1][y1]);
 }
 trap_count ++;
 }
attroff(COLOR_PAIR(TRAP_COLOR_2));

attron (COLOR_PAIR(HEAL_OFF));
while  (heal_count < heal_percentage) {
 int x1 = rand() % (map->x - 2) + 1;
 int y1 = rand() % (map->y - 1);
 if (map->matrix [x1] [y1] != '#') {
  map->matrix[x1][y1] = '+';
  mvaddch(y1, x1, map->matrix[x1][y1]);
 }
 heal_count ++;
 }
attroff(COLOR_PAIR(HEAL_OFF));

attron (COLOR_PAIR(BULLET_OFF));
while  (bullet_count < bullet_percentage) {
 int x1 = rand() % (map->x - 2) + 1;
 int y1 = rand() % (map->y - 1);
 if (map->matrix [x1] [y1] != '#') {
  map->matrix[x1][y1] = '-';
  mvaddch(y1, x1, map->matrix[x1][y1]);
 }
 bullet_count ++;
 }
attroff(COLOR_PAIR(BULLET_OFF));
// #endregion
}
