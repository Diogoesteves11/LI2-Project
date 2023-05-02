#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "mapa.h"

#define WALL_PAIR 0

void draw_map(STATE *s, MAPA *map) {
    s->playerX = 10;
    s->playerY = 10;
    int casas_totais = ((map->x) * (map->y)) - 2 * (map->x + map->y);

    start_color();
    init_pair(WALL_PAIR,COLOR_BLACK,COLOR_BLACK); // Cor preta iniciada

    attron(COLOR_PAIR(0));  // Cor preta aplicada às paredes, para que estas estejam "sem iluminação".
    for (int i = 0; i < casas_totais * 0.4; i++) {
        int x1 = rand() % (map->x - 2) + 1;
        int y1 = rand() % (map->y - 1);
        map->matriz[x1][y1] = '#';
        mvaddch(y1, x1, map->matriz[x1][y1]);
    }
    attroff(COLOR_PAIR(0));

    // Adiciona paredes nas bordas do mapa
    attron(COLOR_PAIR(0));
    for (int i = 0; i < map->x; i++) {
        map->matriz[i][0] = '#'; // Parede superior
        map->matriz[i][map->y - 2] = '#'; // Parede inferior (uma linha acima da última)
        mvaddch(0, i, map->matriz[i][0]);
        mvaddch(map->y - 2, i, map->matriz[i][map->y - 2]);
    }
    attroff(COLOR_PAIR(0));

    attron(COLOR_PAIR(0));
    for (int i = 0; i < map->y - 1; i++) {
        map->matriz[0][i] = '#'; // Parede esquerda
        map->matriz[map->x - 1][i] = '#'; // Parede direita
        mvaddch(i, 0, map->matriz[0][i]);
        mvaddch(i, map->x - 1, map->matriz[map->x - 1][i]);
        
    }
    attroff(COLOR_PAIR(0));

    // Limpa a última linha (linha inferior)
    for (int i = 1; i < map->x - 1; i++) {
        map->matriz[i][map->y - 1] = ' '; // Deixa a célula vazia
        mvaddch(map->y - 1, i, map->matriz[i][map->y - 1]);
    }

    // Transforma todas as posições em paredes
    attron(COLOR_PAIR(0));
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
    attroff(COLOR_PAIR(0));
// Verifica cada posição em um quadrado 5x5 centrado e transforma em parede se não houver nenhuma parede vizinha
// faz se 7 passagens para um resultado mais redondo
attron(COLOR_PAIR(0));
for(int d = 0; d < 7; d++){
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
attroff(COLOR_PAIR(0));
}

