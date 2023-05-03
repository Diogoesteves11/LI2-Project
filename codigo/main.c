#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "state.h"
#include "mapa.h"
#include "mapa.c"

#define BACKGROUND 0
#define WALL_ILUMINATED 1
#define PLAYER 2
#define LIGHT 3
#define SCORE 4 
#define WALL_BASE 5     // definem-se numeros para a definição das cores dos diferentes elementos do jogo


void draw_light (STATE *s, MAPA *map){ // Função que desenhará a luz
  int centerX = s->playerY;  
    int centerY = s->playerX; 
    char test = '#';

    for (int x = centerY - 2; x <= centerY + 2; x++) {
        for (int y = centerX - 2; y <= centerX + 2; y++) {
            if ((mvinch(x,y) & A_CHARTEXT) == test) {
                attron(COLOR_PAIR(WALL_ILUMINATED));
                mvaddch(x, y, '#');
                attroff(COLOR_PAIR(WALL_ILUMINATED));
                }
        }
    }
}


void do_movement_action(STATE *st, int dx, int dy) {
	int nextX = st->playerX + dx;
    int nextY = st->playerY + dy; 
	char test = '#';
    if ((mvinch(nextX, nextY) && A_CHARTEXT) == test){
     return;
	} 
    st->playerX = nextX;
    st->playerY = nextY;
}

void update(STATE *st) {
	int key = getch();

	mvaddch(st->playerX, st->playerY, ' ');
	switch(key) {
		case KEY_A1:
		case '7': do_movement_action(st, -1, -1); break;
		case KEY_UP:
		case '8': do_movement_action(st, -1, +0); break;
		case KEY_A3:
		case '9': do_movement_action(st, -1, +1); break;
		case KEY_LEFT:
		case '4': do_movement_action(st, +0, -1); break;
		case KEY_B2:
		case '5': break;
		case KEY_RIGHT:
		case '6': do_movement_action(st, +0, +1); break;
		case KEY_C1:
		case '1': do_movement_action(st, +1, -1); break;
		case KEY_DOWN:
		case '2': do_movement_action(st, +1, +0); break;
		case KEY_C3:
		case '3': do_movement_action(st, +1, +1); break;
		case 'q': endwin(); exit(0); break;
		case 'w': do_movement_action(st, -1, +0); break;
		case 'a': do_movement_action(st, +0, -1); break;
		case 's': do_movement_action(st, +1, +0); break;
		case 'd': do_movement_action(st, +0, +1); break;
	}
}

int main() {
	MAPA map;
	STATE st = {20,20};
	WINDOW *wnd = initscr();
	int ncols, nrows;
	getmaxyx(wnd,nrows,ncols);
   

	srand48(time(NULL));
	start_color();

	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, false);
	keypad(stdscr, true);

	init_pair(WALL_ILUMINATED, COLOR_WHITE, COLOR_BLACK);
    init_pair(LIGHT, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SCORE, COLOR_BLUE, COLOR_BLACK);
	init_pair(PLAYER, COLOR_GREEN, COLOR_BLACK);
	init_pair(BACKGROUND, COLOR_BLACK,COLOR_BLACK);
	init_pair(WALL_BASE, COLOR_BLACK, COLOR_BLACK);

    map.y = nrows;
	map.x = ncols;
	
	attron(COLOR_PAIR(WALL_BASE));
	draw_map(&st,&map);
	attroff(COLOR_PAIR(WALL_BASE));
	
	while(1) {
		move(nrows - 1, 0);
		attron(COLOR_PAIR(4));
		printw("(%d, %d) %d %d", st.playerX, st.playerY, ncols, nrows);
		attroff(COLOR_PAIR(4));

		attron(COLOR_PAIR(2));
		mvaddch(st.playerX, st.playerY, '@' | A_BOLD);
		attroff(COLOR_PAIR(2));
		draw_light(&st,&map);

		move(st.playerX, st.playerY);
		update(&st);
	}

	return 0;
}
