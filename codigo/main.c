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


#define NORTH 19
#define SOUTH 20
#define EAST 21
#define WEST 22



int distance_player_point (STATE *s, int *x, int *y){
	int dist = sqrt(((s->playerX - *x)^2)+ ((s->playerY - *y)^2));
	return dist;
}

void draw_light(STATE *s,MAPA *map){ // Função que desenhará a luz

int centerX = s->playerY;
int centerY = s->playerX;

char test = '#';
char trap = '*';
char heal = '+';
char bullet = '-';
char casa = ' ';

double delta = 0.05; // Incremento do angulo

    for (double angle = 0; angle < 2 * M_PI; angle += delta) {

         double dx = cos(angle);
         double dy = sin(angle);

        // posição inicial
        double x = centerX + 0.5; // incrementa-se 0.5 para se arredondar o valor para cima
        double y = centerY + 0.5;

        // percorre a direção dada pelo raio até à borda do mapa ou até encontrar um obstáculo(parede)
        while (x >= 0 && x < map->x && y >= 0 && y < map->y) {
            char testch = mvinch((int)y, (int)x) & A_CHARTEXT;
            if (testch == test) {
                attron(COLOR_PAIR(WALL_ILUMINATED));
                mvaddch(y, x, '#');
                attroff(COLOR_PAIR(WALL_ILUMINATED));
                break;
            }
            else if (testch == trap) {
                attron(COLOR_PAIR(TRAP_COLOR));
                mvaddch(y, x, '*' | A_BOLD);
                attroff(COLOR_PAIR(TRAP_COLOR));
                
            }
            else if (testch == heal) {
                attron(COLOR_PAIR(HEAL_ON));
                mvaddch(y, x, '+' | A_BOLD);
                attroff(COLOR_PAIR(HEAL_ON));
                
            }
            else if (testch == bullet) {
                attron(COLOR_PAIR(BULLET_ON));
                mvaddch(y, x, '-' | A_BOLD);
                attroff(COLOR_PAIR(BULLET_ON));
                
            }
            else if (testch == casa) {
                attron(COLOR_PAIR(LIGHT));
                mvaddch(y, x, '.' | A_BOLD);
                attroff(COLOR_PAIR(LIGHT));
            }
            x += dx;
            y += dy;
        }
    }
}

	
void lights_off(MAPA *map){

	char casa_iluminada = '.';

	for (int x = 1; x < map->x -1; x++){ // ciclo que apagará as luzes da jogada anterior
		for (int y = 1; y < map->y-1; y++){
			char testch = (mvinch(y,x) & A_CHARTEXT);
			if (testch == casa_iluminada){
				attron(COLOR_PAIR(BACKGROUND));
				mvaddch(y,x,' ');
				attroff(COLOR_PAIR(BACKGROUND));
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

void shoot(STATE *s, int *direction){
  switch (*direction){
	case NORTH:{
		char testch;
		int y = s->playerY, x = s->playerX;
		for (;(testch =(mvinch (y,x))) == ' '; y--){
          if (s->bullets > 0){
			 attron(COLOR_PAIR(MEDIUM_HP));
		     mvaddch(y,x,'|');
		     attroff(COLOR_PAIR(MEDIUM_HP));
		     s->bullets --;
		  }
		}
	}
	default: break;
  }
}

void update(STATE *st){
	int key = getch();
	int last_direction;

	switch (key)
	{
	case KEY_A1:
	case '7':
		do_movement_action(st, -1, -1);last_direction = WEST;
		break;
	case KEY_UP:
	case '8':
		do_movement_action(st, -1, +0);last_direction = NORTH;
		break;
	case KEY_A3:
	case '9':
		do_movement_action(st, -1, +1);last_direction = EAST;
		break;
	case KEY_LEFT:
	case '4':
		do_movement_action(st, +0, -1);last_direction = WEST;
		break;
	case KEY_B2:
	case '5':
		break;
	case KEY_RIGHT:
	case '6':
		do_movement_action(st, +0, +1);last_direction = EAST;
		break;
	case KEY_C1:
	case '1':
		do_movement_action(st, +1, -1);last_direction = WEST;
		break;
	case KEY_DOWN:
	case '2':
		do_movement_action(st, +1, +0);last_direction = SOUTH;
		break;
	case KEY_C3:
	case '3':
		do_movement_action(st, +1, +1);last_direction = EAST;
		break;
	case 'q':
		endwin();
		exit(0);
		break;
	case 'w':
		do_movement_action(st, -1, +0);last_direction = NORTH;
		break;
	case 'a':
		do_movement_action(st, +0, -1);last_direction = WEST;
		break;
	case 's':
		do_movement_action(st, +1, +0);last_direction = SOUTH;
		break;
	case 'd':
		do_movement_action(st, +0, +1);last_direction = EAST;
		break;
	case '\0':
		break;
	case ' ': shoot(st,&last_direction);break;
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
        
		draw_light(&st,&map);
		move(st.playerX, st.playerY);
		update(&st);
		lights_off(&map);
		
		
	}

	return 0;
}
