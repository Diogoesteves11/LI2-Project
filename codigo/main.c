#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "state.h"
#include "mapa.h"
#include "mapa.c"

#define BACKGROUND 0
#define WALL_ILUMINATED 1
#define PLAYER 2
#define LIGHT 3
#define SCORE 4
#define WALL_BASE 5
#define TRAP_COLOR 6
#define MEDIUM_HP 11
#define HEAL_ON 13
#define BULLET_ON 17
#define CASE_COLOR 18


void draw_light(STATE *s, MAPA *map){ // Função que desenhará a luz

    int centerX = s->playerY;
	int centerY = s->playerX;

	char test = '#';
	char trap = '*';
	char heal = '+';
	char bullet = '-';
	char casa = ' ';

	for (double angle = 0; angle < (M_PI * 2); angle += (M_PI / 8)){ // este ciclo desenha a nova luz
		int centerX1 = s->playerY;
		int centerY1 = s->playerX;
		int dx = 0, dy = 0;

		if ((0 < angle) && (angle < M_PI / 2))
		{
			dx = 1;
			dy = 1;
		}
		else if ((M_PI / 2 < angle) && (angle < M_PI))
		{
			dx = -1;
			dy = 1;
		}
		else if ((M_PI < angle) && (angle < (3 * M_PI) / 2))
		{
			dx = -1;
			dy = -1;
		}
		else if (((3 * M_PI) / 2 < angle) && (angle < 2* M_PI)){
			dx = 1;
			dy = -1;
		}
        else if ((angle == 0)) {
			dx = 1;
			dy = 0;
		}
		else if ((angle == M_PI / 2)){
			dx = 0;
			dy = 1;
		}
		else if ((angle == (3 * M_PI) / 2)){
			dx = 0;
			dy = -1;
		}
		else if ((angle == M_PI)){
			dx = -1;
			dy = 0;
		}

		int x = centerX, y = centerY;
		char testch1 = ' ';
        
   while (testch1 != '#'){
	    x += dx;
		y += dy;
		testch1 = mvinch(y, x) & A_CHARTEXT;
		if (testch1 == casa){
			attron(COLOR_PAIR(LIGHT));
			mvaddch(y, x, '.');
			attroff(COLOR_PAIR(LIGHT));
		}
		else if (testch1 == trap){
			attron(COLOR_PAIR(TRAP_COLOR));
			mvaddch(y, x, '*' | A_BOLD);
			attroff(COLOR_PAIR(TRAP_COLOR));
		}
		else if (testch1 == heal){
	        attron(COLOR_PAIR(HEAL_ON));
			mvaddch(y, x, '+' | A_BOLD);
			attroff(COLOR_PAIR(HEAL_ON));
		}
		else if (testch1 == bullet){
		    attron(COLOR_PAIR(BULLET_ON));
			mvaddch(y, x, '-' | A_BOLD);
			attroff(COLOR_PAIR(BULLET_ON));
		}
   }
 }
}
	
void do_movement_action(STATE *st, int dx, int dy){
	int nextX = st->playerX + dx;
	int nextY = st->playerY + dy;
	char test, testch, testTrap = '*';
	test = '#';
	char heal = '+';
	char bullet = '-';
	testch = (mvinch(nextX, nextY) & A_CHARTEXT);
	if (testch == test)
	{
		return;
	}
	else if (testch == testTrap)
	{
		if (st->hp == 0)
		{
			erase();
			mvprintw(0, 0, "You stepped on a trap and died!");
			refresh();
			sleep(1);
			endwin();
			exit(0);
		}
		else
			st->hp--;
	}
	else if (testch == bullet)
		st->bullets += 5; // cada recarga aumenta 5 balas
	else if (testch == heal)
		st->hp += 2; // cada cura aumenta 2 de hp
	mvaddch(st->playerX, st->playerY, ' ');
	st->playerX = nextX;
	st->playerY = nextY;
}

void update(STATE *st){
	int key = getch();

	switch (key)
	{
	case KEY_A1:
	case '7':
		do_movement_action(st, -1, -1);
		break;
	case KEY_UP:
	case '8':
		do_movement_action(st, -1, +0);
		break;
	case KEY_A3:
	case '9':
		do_movement_action(st, -1, +1);
		break;
	case KEY_LEFT:
	case '4':
		do_movement_action(st, +0, -1);
		break;
	case KEY_B2:
	case '5':
		break;
	case KEY_RIGHT:
	case '6':
		do_movement_action(st, +0, +1);
		break;
	case KEY_C1:
	case '1':
		do_movement_action(st, +1, -1);
		break;
	case KEY_DOWN:
	case '2':
		do_movement_action(st, +1, +0);
		break;
	case KEY_C3:
	case '3':
		do_movement_action(st, +1, +1);
		break;
	case 'q':
		endwin();
		exit(0);
		break;
	case 'w':
		do_movement_action(st, -1, +0);
		break;
	case 'a':
		do_movement_action(st, +0, -1);
		break;
	case 's':
		do_movement_action(st, +1, +0);
		break;
	case 'd':
		do_movement_action(st, +0, +1);
		break;
	case '\0':
		break;
	default:
		break;
	}
}

int main(){
	MAPA map;
	STATE st = {20, 20, 3, 0};
	WINDOW *wnd = initscr();
	int ncols, nrows;
	getmaxyx(wnd, nrows, ncols);

	srand48(time(NULL));
	start_color();

	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, false);
	keypad(stdscr, true);

	init_pair(WALL_ILUMINATED, COLOR_WHITE, COLOR_BLACK);
	init_pair(LIGHT, COLOR_WHITE, COLOR_BLACK);
	init_pair(SCORE, COLOR_BLUE, COLOR_BLACK);
	init_pair(PLAYER, COLOR_GREEN, COLOR_BLACK);
	init_pair(BACKGROUND, COLOR_BLACK, COLOR_BLACK);
	init_pair(TRAP_COLOR, COLOR_RED, COLOR_BLACK);
	init_pair(MEDIUM_HP, COLOR_YELLOW, COLOR_BLACK);
	init_pair(HEAL_ON, COLOR_GREEN, COLOR_BLACK);
	init_pair(BULLET_ON, COLOR_YELLOW, COLOR_BLACK);
	init_pair(BACKGROUND, COLOR_BLACK, COLOR_BLACK);

	map.y = nrows;
	map.x = ncols;

	draw_map(&st, &map);

	while (1)
	{
		move(nrows - 1, 0);
		attron(COLOR_PAIR(1));
		printw("(%d, %d) %d %d", st.playerX, st.playerY, ncols, nrows);
		printw("    Bullets: %d", st.bullets);
		attroff(COLOR_PAIR(1));

		if (st.hp > 1)
		{
			attron(COLOR_PAIR(2));
			mvaddch(st.playerX, st.playerY, '@' | A_BOLD);
			attroff(COLOR_PAIR(2));
		}
		else if (st.hp == 1)
		{
			attron(COLOR_PAIR(MEDIUM_HP));
			mvaddch(st.playerX, st.playerY, '@' | A_BOLD);
			attroff(COLOR_PAIR(MEDIUM_HP));
		}
		else
		{
			attron(COLOR_PAIR(TRAP_COLOR));
			mvaddch(st.playerX, st.playerY, '@' | A_BOLD);
			attroff(COLOR_PAIR(TRAP_COLOR));
		}

		draw_light(&st, &map);
		move(st.playerX, st.playerY);
		update(&st);
		
	}

	return 0;
}
