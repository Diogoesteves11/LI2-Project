#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "mapa.h"

#define TRAP_PERCENTAGE 10
#define TRAP_COLOR_2 8
#define WALL_COLOR 7
#define HEAL 12

void draw_map(STATE* s, MAPA* map) {
    start_color();

    init_pair(TRAP_COLOR_2, COLOR_BLACK, COLOR_BLACK);
    init_pair(WALL_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(HEAL,COLOR_GREEN,COLOR_BLACK);

    s->playerX = 10;
    s->playerY = 10;
    int casas_totais = ((map->x) * (map->y)) - 2 * (map->x + map->y);
    int heal_percentage = casas_totais * 0.05;
    int heal_count = 0;

    attron(COLOR_PAIR(WALL_COLOR));
    for (int i = 0; i < casas_totais * 0.4; i++) {
        int x1 = rand() % (map->x - 2) + 1;
        int y1 = rand() % (map->y - 1);
        map->matriz[x1][y1] = '#';
        mvaddch(y1, x1, map->matriz[x1][y1]);
    }

    // Adiciona paredes nas bordas do mapa
   
    for (int i = 0; i < map->x; i++) {
        map->matriz[i][0] = '#'; // Parede superior
        map->matriz[i][map->y - 2] = '#'; // Parede inferior (uma linha acima da última)
        mvaddch(0, i, map->matriz[i][0]);
        mvaddch(map->y - 2, i, map->matriz[i][map->y - 2]);
    }
    for (int i = 0; i < map->y - 1; i++) {
        map->matriz[0][i] = '#'; // Parede esquerda
        map->matriz[map->x - 1][i] = '#'; // Parede direita
        mvaddch(i, 0, map->matriz[0][i]);
        mvaddch(i, map->x - 1, map->matriz[map->x - 1][i]);
    }
  

    // Limpa a última linha (linha inferior)
    for (int i = 1; i < map->x - 1; i++) {
        map->matriz[i][map->y - 1] = ' '; // Deixa a célula vazia
        mvaddch(map->y - 1, i, map->matriz[i][map->y - 1]);
    }

    // Transforma algumas posições em paredes se existirem pelo menos 5 paredes em um quadrado 3x3 centrado em cada posição
    
    for (int d = 0; d < 2; d++) {
    for (int i = 1; i < map->x - 1; i++) {
        for (int j = 1; j < map->y - 1; j++) {
            int count = 0;
            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    if (map->matriz[i + k][j + l] == '#') {
                        count++;
                    }
                }
            }
            if (count >= 5) {
                map->matriz[i][j] = '#';
                mvaddch(j, i, map->matriz[i][j]);
            }
        }
    }
    }
    
// Verifica cada posição em um quadrado 5x5 centrado e transforma em parede se não houver nenhuma parede vizinha
// faz-se 7 passagens para um resultado mais polido

for (int d = 0; d < 7; d++) {
    for (int i = 2; i < map->x - 2; i++) {
        for (int j = 2; j < map->y - 2; j++) {
            int count = 0;
            for (int k = -2; k <= 2; k++) {
                for (int l = -2; l <= 2; l++) {
                    if (map->matriz[i + k][j + l] == '#') {
                        count++;
                    }
                }
            }
            if (count == 0) {
                map->matriz[i][j] = '#';
                mvaddch(j, i, map->matriz[i][j]);
            }
        }
    }
}

// Elimina paredes soltas
for (int i = 1; i < map->x - 1; i++) {
    for (int j = 1; j < map->y - 1; j++) {
        if (map->matriz[i][j] == '#') {
            int count = 0;
            if (map->matriz[i - 1][j] == '#') count++; // Verifica parede acima
            if (map->matriz[i + 1][j] == '#') count++; // Verifica parede abaixo
            if (map->matriz[i][j - 1] == '#') count++; // Verifica parede à esquerda
            if (map->matriz[i][j + 1] == '#') count++; // Verifica parede à direita

            if (count == 0) {
                map->matriz[i][j] = ' '; // Remove a parede solta
                mvaddch(j, i, map->matriz[i][j]);
            }
        }
    }
}
attroff(COLOR_PAIR(WALL_COLOR));

attron(COLOR_PAIR(TRAP_COLOR_2));
for(int i = 2; i < map->y-2; i++) {
        for(int j = 2; j < map->x-2; j++) {
            char test = '#';
            if((drand48() * 1000 < TRAP_PERCENTAGE) && ((mvinch(j,i) && A_CHARTEXT) != test)) {
                mvaddch(i, j, '*'| A_BOLD);
            } 
        }
    }
attroff (COLOR_PAIR(TRAP_COLOR_2));

attron (COLOR_PAIR(HEAL));
for (int i = 2; i < map->y-2; i++){
    for (int j = 2; j < map-> x -2; j++){
        if ((map->matriz[i][j] == ' ') && (heal_count < heal_percentage)){
            mvaddch(i,j, '+'|A_BOLD);
            heal_count ++;
        }
    }
}
attroff(COLOR_PAIR(HEAL));

}