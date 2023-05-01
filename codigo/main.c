#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "state.h"
#include "mapa.h"
#include "mapa.c"


void draw_light (STATE *s, MAPA *map){ // Função que desenhará a luz

}

void do_movement_action(STATE *st, int dx, int dy) {
	int nextX = st->playerX + dx;
    int nextY = st->playerY + dy; 
    if (mvinch(nextX, nextY) == '#') return;
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

	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);

    map.y = nrows;
	map.x = ncols;
	draw_map(&st,&map);
	
	while(1) {
		move(nrows - 1, 0);
		attron(COLOR_PAIR(COLOR_BLUE));
		printw("(%d, %d) %d %d", st.playerX, st.playerY, ncols, nrows);
		attroff(COLOR_PAIR(COLOR_BLUE));
		attron(COLOR_PAIR(COLOR_WHITE));
		mvaddch(st.playerX, st.playerY, '@' | A_BOLD);
		attron(COLOR_PAIR(COLOR_GREEN));
		move(st.playerX, st.playerY);
		draw_light(&st,&map);
		update(&st);
	}

	return 0;
}
