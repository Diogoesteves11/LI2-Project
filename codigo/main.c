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
#define CASE_COLOR 18 // definições das "referências" das cores
#define ENEMIE_COLOR 19

#define N 19 // definição das "referências" das direções
#define S 20
#define E 21
#define W 22
#define NW 23
#define SW 24
#define NE 25
#define SE 26
#define NO_DIRECTION 27

void spawn_player(STATE *st, MAPA *map) { // função que escolhe ateatóriamente um local do mapa para o jogador aparecer, sem paredes imediatamente à sua volta
    int x, y;
    // Escolhe uma posição aleatória dentro do mapa
    do {
        x = (rand() % (map->x - 2)) + 1;
        y = (rand() % (map->y - 3)) + 2;
    } while (map->matriz[y][x] !=' '|| // posição já ocupada
             map->matriz[y-1][x] == '#' || // parede acima
             map->matriz[y+1][x] == '#' || // parede abaixo
             map->matriz[y][x-1] == '#' || // parede à esquerda
             map->matriz[y][x+1] == '#'); // parede à direita

    st->playerX = y;
    st->playerY = x;

}

void draw_light(STATE *s,MAPA *map){ // Função que desenhará a luz

int centerX = s->playerX;
int centerY = s->playerY;

char test = '#';
char trap = '*';
char heal = '+';
char bullet = '-';
char casa = ' ';
char enemy = '&';
char damage = '^';

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
                map-> matriz [(int)x][(int)y] = test;
                attron(COLOR_PAIR(WALL_ILUMINATED));
                mvaddch(y, x, '#');
                attroff(COLOR_PAIR(WALL_ILUMINATED));
                break;
            }
            else if (testch == trap) {
                map-> matriz [(int)x][(int)y] = trap;
                attron(COLOR_PAIR(TRAP_COLOR));
                mvaddch(y, x, '*' | A_BOLD);
                attroff(COLOR_PAIR(TRAP_COLOR));
                
            }
            else if (testch == heal) {
                map-> matriz [(int)x][(int)y] = heal;
                attron(COLOR_PAIR(HEAL_ON));
                mvaddch(y, x, '+' | A_BOLD);
                attroff(COLOR_PAIR(HEAL_ON));
                
            }
            else if (testch == bullet) {
                map-> matriz [(int)x][(int)y] = bullet;
                attron(COLOR_PAIR(BULLET_ON));
                mvaddch(y, x, '-' | A_BOLD);
                attroff(COLOR_PAIR(BULLET_ON));
                
            }
            else if (testch == casa) {
                map->matriz[(int)x][(int)y] = '.';
                attron(COLOR_PAIR(LIGHT));
                mvaddch(y, x, '.' | A_BOLD);
                attroff(COLOR_PAIR(LIGHT));
            }
            else if (testch == enemy){
             map-> matriz [(int)x][(int)y] = enemy;
             attron(COLOR_PAIR(ENEMIE_COLOR));
             mvaddch(y, x, '&' | A_BOLD);
             attroff(COLOR_PAIR(ENEMIE_COLOR));
            }
             else if (testch == damage){
             map-> matriz [(int)x][(int)y] = damage;
             attron(COLOR_PAIR(TRAP_COLOR));
             mvaddch(y, x, '.' | A_BOLD);
             attroff(COLOR_PAIR(TRAP_COLOR));
             map->matriz[(int)x][(int)y] = '.';
            }
            x += dx;
            y += dy;
        }
    }
}

void lights_off(MAPA *map) { // função que apaga a luz da jogada anterior
    char casa_iluminada = '.';
    char trap = '*';
	char enemy = '&';
    char damage = '^';

    attron(COLOR_PAIR(BACKGROUND));
    for (int x = 1; x < map->x - 1; x++) { // ciclos for para percorrer todas as casas do mapa
        for (int y = 1; y < map->y - 1; y++) {
            char testch = map->matriz[x][y];
            if (testch == casa_iluminada) {  //  se a casa estiver iluminada, é apagada para a mesma cor do background, bem como as traps (cor preta)
                map->matriz[x][y] = ' ';   
				attron(COLOR_PAIR(BACKGROUND));         
				mvaddch(y, x,map->matriz[x][y]);
				attroff(COLOR_PAIR(BACKGROUND));
            }
            else if (testch == trap) {
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);  // chama-se esta função para garantir que as traps sejam pintadas de preto
                mvaddch(y, x, '*' | A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
			else if (testch == enemy) {
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);  // chama-se esta função para garantir que os inimigos sejam pintadas de preto
                mvaddch(y, x, enemy|A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
            else if (testch == damage){
                map->matriz[x][y] = '^';
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);  // chama-se esta função para garantir que os inimigos sejam pintadas de preto
                mvaddch(y, x, map->matriz[x][y]|A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
        }
    }
    attroff(COLOR_PAIR(BACKGROUND));
}

void do_movement_action(STATE *st, int dx, int dy,MAPA *map, int *game_menu){  // função que dará "fisica" ao jogador, fazendo com que ele interaja com os obstáculos que intersetar
	int nextX = st->playerX + dx;
	int nextY = st->playerY + dy;
	char testTrap = '*';
	char test = '#';
	char heal = '+';
	char bullet = '-';
	char enemich = '&';
	char testch = map->matriz[nextX][nextY];
	if (testch == test) return; // se a próxima posição for uma parede, o jogador não se mexe e a função retorna 
	else if (testch == testTrap) // se a próxima posição for uma trap, verificamos o hp do jogador e, se esta for maior do que zero, retiramos 1 de hp, caso contrário o jogador morre
	{
		if (st->hp == 0)
		{
			erase();
			mvprintw(0, 0, "YOU DIED!");
			refresh();
			sleep(1);
            endwin();
            (*game_menu = 1);
			
		}
		else
			st->hp--;
	}
	else if (testch == bullet) st->bullets += 5; // cada recarga aumenta 5 balas, caso o jogador intersete a munição
	else if (testch == heal) st->hp += 2; // cada cura aumenta 2 de hp, caso o jogador a intersete
	else if (testch == enemich) return;
	mvaddch(st->playerY, st->playerX, ' ');
	st->playerX = nextX; // atualizamos as novas coordenadas do jogador no state do mesmo
	st->playerY = nextY;
}

void update(STATE *st,MAPA *map,int *game_menu){ // função que fornecerá à "do_movement_action" as informações acerca da próxima jogador do jogador
	int key = getch();
	int direction = NO_DIRECTION;

	switch (key)
	{
     case 't':
        // se o jogador não tiver a espada equipada, equipa a espada
        if (!st->sword) {
            st->sword = 1;
        } 
        // caso contrário, desequipa a espada e equipa a arma
        else {
            st->sword = 0;
        }
        break;

	case KEY_A1:
	case '7':
		do_movement_action(st, -1, -1,map,game_menu);direction = NW;
		break;
	case KEY_UP:
	case '8':
		do_movement_action(st, +0, -1,map,game_menu);direction = N;
		break;
	case KEY_A3:
	case '9':
		do_movement_action(st, +1, -1,map,game_menu);direction = NE;
		break;
	case KEY_LEFT:
	case '4':
		do_movement_action(st, -1, +0,map,game_menu);direction = W;
		break;
	case KEY_B2:
	case '5': direction = NO_DIRECTION;break; // para fazer uma jogada parado
	case KEY_RIGHT:
	case '6':
		do_movement_action(st, +1, +0,map,game_menu);direction = E;
		break;
	case KEY_C1:
	case '1':
		do_movement_action(st, -1, +1,map,game_menu);direction = SW;
		break;
	case KEY_DOWN:
	case '2':
		do_movement_action(st, +0, +1,map,game_menu);direction = S;
		break;
	case KEY_C3:
	case '3':
		do_movement_action(st, +1, +1,map,game_menu);direction = SE;
		break;
	case 'q':
        *game_menu = 1;
		break;
	case 'w':
		do_movement_action(st, +0, -1,map,game_menu);direction = N;
		break;
	case 'a':
		do_movement_action(st, -1, +0,map,game_menu);direction = W;
		break;
	case 's':
		do_movement_action(st, +0, +1,map,game_menu);direction = S;
		break;
	case 'd':
		do_movement_action(st, +1, +0,map,game_menu);direction = E;
		break;
	}
}

void show_menu() {
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);


    clear();
    
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  // Obtém as dimensões da tela
    
    int menu_width = 20;
    int menu_height = 6;
    int start_x = (cols - menu_width) / 2;
    int start_y = (rows - menu_height) / 2;
    

    attron(COLOR_PAIR(1));
    mvprintw(start_y, start_x, "====================");
    mvprintw(start_y + 1, start_x, "     START MENU");
    mvprintw(start_y + 2, start_x, "====================");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(start_y + 3, start_x, " 1. PLAY GAME");
    mvprintw(start_y + 4, start_x, " 2. COMMAND LIST");
    mvprintw(start_y + 5, start_x, " 3. EXIT");
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(1));
    mvprintw(start_y + 6, start_x, "====================");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(start_y + 7, start_x, "  CHOOSE AN OPTION ");
    attroff(COLOR_PAIR(2));

    refresh();
}

void refresh_GAME_STATUS(MAPA *map){
    for (int ix = 0; ix <= map->x; ix++){
        for (int iy = 0; iy <= map->y; iy++){
         map->matriz[ix][iy] = '\0';
        }
    }
}

void spawn_monsters(MOSNTERS *monster, MAPA *map){
  char casa = ' ';
  int ix,iy;
  for (int i = 0; i < 20; i++){
   do{
   ix = 1 + (rand() % (map-> x) / 2);
   iy = 1 + (rand() % (map-> y) / 2);
   }while(map->matriz[ix][iy]!= casa);
   map->matriz[ix][iy] = '&';
   monster[i].y = iy;
   monster[i].x = ix;
   monster[i].hp = 2;
  }
}

int main() {
  int in_game = 0;
  int in_submenu = 0;
  MAPA map;

  while (1) {
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    start_color();
    init_pair(LIGHT, COLOR_WHITE, COLOR_BLACK);

    while (1) {
      show_menu(); // Exibir menu inicial

      int choice = 0;
      choice = getch();
      clear();

      switch (choice) {
        case '1': {
          in_game = 1; // Indica que o jogo está em execução
          refresh_GAME_STATUS(&map);
          while (in_game) {
            STATE st = {20, 20, 3, 0, 1, 0};
            MOSNTERS monster[20];
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
            init_pair(ENEMIE_COLOR, COLOR_MAGENTA, COLOR_BLACK);

            map.y = nrows;
            map.x = ncols;

            draw_map(&map);
            spawn_player(&st, &map);
            spawn_monsters(monster,&map);
           
            while (in_game) {
                int game_menu = 0;
              move(nrows - 1, 0);
              attron(COLOR_PAIR(1));
              printw("(%d, %d) %d %d", st.playerX, st.playerY, ncols, nrows);
              clrtoeol();
              printw("    Bullets: %d", st.bullets);
              clrtoeol();
              printw("   HP: %d", (st.hp + 1));
              clrtoeol();
              printw("   KILLS: %d", st.kills);
              clrtoeol();
              printw("   EQUIPPED:");
              if (st.sword) {
                printw(" SWORD ");
              } else {
                printw(" GUN");
              }
              attroff(COLOR_PAIR(1));

              if (st.hp > 1) {
                map.matriz[st.playerX][st.playerY] = '@';
                attron(COLOR_PAIR(2));
                mvaddch(st.playerY, st.playerX, '@' | A_BOLD);
                attroff(COLOR_PAIR(2));
              } else if (st.hp == 1) {
                map.matriz[st.playerX][st.playerY] = '@';
                attron(COLOR_PAIR(MEDIUM_HP));
                mvaddch(st.playerY, st.playerX, '@' | A_BOLD);
                attroff(COLOR_PAIR(MEDIUM_HP));
              } else {
                map.matriz[st.playerX][st.playerY] = '@';
                attron(COLOR_PAIR(TRAP_COLOR));
                mvaddch(st.playerY, st.playerX, '@' | A_BOLD);
                attroff(COLOR_PAIR(TRAP_COLOR));
              }

              lights_off(&map);
              draw_light(&st, &map);
              move(st.playerY, st.playerX);
              update(&st,&map, &game_menu);

              if (game_menu) {
                in_game = 0; // Sair do jogo atual e voltar ao menu principal
              }
            }
          }

          break;
        }
        case '2':{
         in_submenu = 1;
         while (in_submenu){
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

            init_pair(LIGHT,COLOR_WHITE,COLOR_BLACK);
            init_pair(2,COLOR_GREEN,COLOR_BLACK);

            int startx = ncols /20;
            int starty = nrows/20;

            attron(COLOR_PAIR(LIGHT)); 
            mvprintw(starty,startx,"t -> switch gun");
            mvprintw(starty+1,startx,"w/8 -> move UP");
            mvprintw(starty+2,startx,"s/2 -> move DOWN");
            mvprintw(starty+3,startx,"a/4 -> move LEFT");
            mvprintw(starty+4,startx,"d/6 -> move RIGHT");
            mvprintw(starty+5,startx,"7 -> move UPPER LEFT");
            mvprintw(starty+6,startx,"9 -> move UPPER RIGHT");
            mvprintw(starty+7,startx,"1 -> move DOWN LEFT");
            mvprintw(starty+8,startx,"3 -> move DOWN RIGHT");
            mvprintw(starty+9,startx,"SPACE -> USE WEAPON");
            mvprintw(starty+10,startx,"5-> NO MOVEMENT");
            attroff(COLOR_PAIR(LIGHT));


            int key = getch();
            switch(key){
             case 'q': in_submenu = 0;break;
            }
           }
           break;
        }
        case '3': {
          endwin();
          printf("GAME CLOSED\n");
          exit(0);
          break;
        }
      }
    }
  }

  return 0;
}

             
