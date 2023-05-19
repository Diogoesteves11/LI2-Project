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

// Função que desenha o mapa
void draw_map(MAPA* map) {
    srand(time(NULL)); // Mapas aleatorios a cada jogo
    start_color();
    init_pair(TRAP_COLOR_2, COLOR_BLACK, COLOR_BLACK);
    init_pair(WALL_COLOR, COLOR_BLACK, COLOR_BLACK);
    init_pair(HEAL_OFF,COLOR_BLACK,COLOR_BLACK);
    init_pair (BULLET_OFF, COLOR_BLACK,COLOR_BLACK);

   // Adiciona paredes nas bordas do mapa
    int count1 = 0;

    attron(COLOR_PAIR(WALL_COLOR));
    for (int i = 0; i < map->x; i++) {
        map->matriz[i][0] = '#'; // Parede superior
        map->matriz[i][map->y - 2] = '#'; // Parede inferior (uma linha acima da última)
        mvaddch(0, i, map->matriz[i][0]);
        mvaddch(map->y - 2, i, map->matriz[i][map->y - 2]);
        count1++;
    }
    for (int i = 0; i < map->y - 1; i++) {
        map->matriz[0][i] = '#'; // Parede esquerda
        map->matriz[map->x - 1][i] = '#'; // Parede direita
        mvaddch(i, 0, map->matriz[0][i]);
        mvaddch(i, map->x - 1, map->matriz[map->x - 1][i]);
        count1++;
    }
  

    // Limpa a última linha (linha inferior)
    for (int i = 1; i < map->x - 1; i++) {
        map->matriz[i][map->y - 1] = ' '; // Deixa a célula do canto inferior direito vazia
        mvaddch(map->y - 1, i, map->matriz[i][map->y - 1]);
    }

    int casas_totais = ((map->x) * (map->y)) - count1; // Calcula-se o numero de paredes sem contar as bordas.
    int heal_percentage = casas_totais * 0.01;
    int heal_count = 0;
    int trap_count = 0;
    int trap_percentage = casas_totais * 0.01;
    int bullet_count = 0;
    int bullet_percentage = casas_totais * 0.01;

    
    for (int i = 0; i < casas_totais * 0.4; i++) {
        int x1 = 2 + rand() % (map->x - 2); 
        int y1 = 2 + rand() % (map->y - 2);
        map->matriz[x1][y1] = '#';
        mvaddch(y1, x1, map->matriz[x1][y1]);
    }


    // Ciclos para deixar as paredes mais concentradas em forma de "cavernas"
    for (int i = 0; i < 3; i++) { // fazem-se 3 passagens
        for (int y = 1; y < (map->y-1) - 1; y++) {
            for (int x = 1; x < (map->x) - 1; x++) {
                int count = 0;
                for (int yy = y - 1; yy <= y + 1; yy++) {
                    for (int xx = x - 1; xx <= x + 1; xx++) {
                        if (map->matriz[xx][yy] == '#') {
                            count++;
                        }
                    }
                }
                if (map->matriz[x][y] == '#') {
                    if (count < 3) {
                        map->matriz[x][y] = ' ';
                        mvaddch(y,x,map->matriz[x][y]);
                    }
                } else {
                    if (count > 4) {
                        map->matriz[x][y] = '#';
                        mvaddch(y,x,map->matriz[x][y]);
                    }
                }
            }
        }
    }
    
// Ciclos para deixar o mapa mais "polido".
    for (int i = 0; i < 4; i++) { // fazem-se 4 passagens
        for (int y = 1; y < (map->y-1) - 1; y++) {
            for (int x = 1; x < (map->x) - 1; x++) {
                int count = 0;
                for (int iy = y - 1; iy <= y + 1; iy++) {
                    for (int ix = x - 1; ix <= x + 1; ix++) {
                        if (map->matriz[ix][iy] == '#' && (iy != y || ix != x)) {
                            count++;
                        }
                    }
                }
                if (map->matriz[x][y] == '#') {
                    if (count < 3) {
                        map->matriz[x][y] = ' ';
                        mvaddch(y,x,map->matriz[x][y]);
                    }
                }
                if (map->matriz[x][y]== ' ') {
                    if (count > 4) {
                        map->matriz[x][y] = '#';
                        mvaddch(y,x,map->matriz[x][y]);
                    }
                }
            }
        }
    }

// Elimina algumas paredes soltas de forma a deixar um maior espaço livre para o jogador se movimentar pelo mapa
 for (int i = 1; i < map->x - 1; i++) {
    for (int j = 1; j < map->y - 1; j++) {
        if (map->matriz[i][j] == '#') {
            int count2 = 0;
            if (map->matriz[i - 1][j] == '#') count2++; // Verifica parede esquerda
            if (map->matriz[i + 1][j] == '#') count2++; // Verifica parede direita
            if (map->matriz[i][j - 1] == '#') count2++; // Verifica parede acima
            if (map->matriz[i][j + 1] == '#') count2++; // Verifica parede abaixo

            if (count2 == 0) {
                map->matriz[i][j] = ' '; // Remove a parede solta
                mvaddch(j, i, map->matriz[i][j]);
            }
        }
    }
}
attroff(COLOR_PAIR(WALL_COLOR));


// Função que desenha as traps do mapa logo no inicio (número de traps finito)
attron (COLOR_PAIR(TRAP_COLOR_2));
while  (trap_count < trap_percentage) {
 int x1 = rand() % (map->x - 2) + 1;
 int y1 = rand() % (map->y - 1);
 if (map->matriz [x1] [y1] != '#') {
  map->matriz[x1][y1] = 'x';
  mvaddch(y1, x1, map->matriz[x1][y1]);
 }
 trap_count ++;
 }
attroff(COLOR_PAIR(TRAP_COLOR_2));

// Função que desenha as curas (nº finito de curas)
attron (COLOR_PAIR(HEAL_OFF));
while  (heal_count < heal_percentage) {
 int x1 = rand() % (map->x - 2) + 1;
 int y1 = rand() % (map->y - 1);
 if (map->matriz [x1] [y1] != '#') {
  map->matriz[x1][y1] = '+';
  mvaddch(y1, x1, map->matriz[x1][y1]);
 }
 heal_count ++;
 }
attroff(COLOR_PAIR(HEAL_OFF));


// Função que desenha as recargas das munições (nº finito de munições)
attron (COLOR_PAIR(BULLET_OFF));
while  (bullet_count < bullet_percentage) {
 int x1 = rand() % (map->x - 2) + 1;
 int y1 = rand() % (map->y - 1);
 if (map->matriz [x1] [y1] != '#') {
  map->matriz[x1][y1] = '-';
  mvaddch(y1, x1, map->matriz[x1][y1]);
 }
 bullet_count ++;
 }
attroff(COLOR_PAIR(BULLET_OFF));
}
