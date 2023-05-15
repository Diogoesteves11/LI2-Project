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

#define NORTH 19 // definição das "referências" das direções
#define SOUTH 20
#define EAST 21
#define WEST 22

void remove_enemie(ENEMIE *enemies, int *num_enemies, int index){
    (*num_enemies) --;
    if(index != *num_enemies){
        enemies[index] = enemies[*num_enemies];
    }
}

int distance_player_point (STATE *s, int *x, int *y){  // calculo da distância entre o jogador e um determinado ponto em linha reta
	int dist = sqrt(((s->playerX - *x)^2)+ ((s->playerY - *y)^2));
	return dist;
}

void draw_light(STATE *s,MAPA *map){ // Função que desenhará a luz

int centerX = s->playerY;
int centerY = s->playerX;

char test = '#';  // definição das variáveis para os diferentes obstáculos
char trap = '*';
char heal = '+';
char bullet = '-';
char casa = ' ';
char damage = '^';
char enemie = '&';


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
                attron(COLOR_PAIR(WALL_ILUMINATED));  // caso se encontre uma parede, para-se de se desenhar a luz na direção definida
                mvaddch(y, x, '#');
                attroff(COLOR_PAIR(WALL_ILUMINATED));
                break;
            }
            else if (testch == trap) {
                attron(COLOR_PAIR(TRAP_COLOR));    // mesmo que se encontre um obstáculo que não seja uma parede, desenhamos a luz além do mesmos, iluminando-o
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
			else if (testch == damage) {
                attron(COLOR_PAIR(TRAP_COLOR));
                mvaddch(y, x, '.' | A_BOLD);
                attroff(COLOR_PAIR(TRAP_COLOR));
			}
			else if (testch == enemie) {
                attron(COLOR_PAIR(ENEMIE_COLOR));
                mvaddch(y, x, '&' | A_BOLD);
                attroff(COLOR_PAIR(ENEMIE_COLOR));
			}
            x += dx;
            y += dy;
        }
    }
}

void lights_off(MAPA *map) { // função que apaga a luz da jogada anterior
    char casa_iluminada = '.';
    char trap = '*';
	char enemie = '&';

    attron(COLOR_PAIR(BACKGROUND));
    for (int x = 1; x < map->x - 1; x++) { // ciclos for para percorrer todas as casas do mapa
        for (int y = 1; y < map->y - 1; y++) {
            char testch = (mvinch(y, x) & A_CHARTEXT);
            if (testch == casa_iluminada) {  //  se a casa estiver iluminada, é apagada para a mesma cor do background, bem como as traps (cor preta)
                mvaddch(y, x, ' ');
            }
            else if (testch == trap) {
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);  // chama-se esta função para garantir que as traps sejam pintadas de preto
                mvaddch(y, x, '*' | A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
			else if (testch == enemie) {
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);  // chama-se esta função para garantir que as traps sejam pintadas de preto
                mvaddch(y, x, '&' | A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
        }
    }
    attroff(COLOR_PAIR(BACKGROUND));
}

void do_movement_action(STATE *st, int dx, int dy){  // função que dará "fisica" ao jogador, fazendo com que ele interaja com os obstáculos que intersetar
	int nextX = st->playerX + dx;
	int nextY = st->playerY + dy;
	char test, testch, testTrap = '*';
	test = '#';
	char heal = '+';
	char bullet = '-';
	char enemich = '&';
	testch = (mvinch(nextX, nextY) & A_CHARTEXT);
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
			exit(0);
		}
		else
			st->hp--;
	}
	else if (testch == bullet) st->bullets += 5; // cada recarga aumenta 5 balas, caso o jogador intersete a munição
	else if (testch == heal) st->hp += 2; // cada cura aumenta 2 de hp, caso o jogador a intersete
	else if (testch == enemich) return;
	mvaddch(st->playerX, st->playerY, ' ');
	st->playerX = nextX; // atualizamos as novas coordenadas do jogador no state do mesmo
	st->playerY = nextY;
}

void attack (STATE *s, int *num_enemies, ENEMIE *enemies){ // função responsável pela fisica dos ataques
	int x = s->playerX, y = s->playerY;
    char wall = '#', heal = '+', bullets = '-', enemiech = '&';

    if (s->sword) {
        for (int ix = x - 1; ix <= x + 1; ix++) {
            for (int iy = y - 1; iy <= y + 1; iy++) {
                char testch = mvinch(ix, iy) & A_CHARTEXT;

                if (testch != wall && testch != heal && testch != bullets && testch != enemiech) {
                    attron(COLOR_PAIR(TRAP_COLOR));
                    mvaddch(ix, iy, '^' | A_BOLD);
                    attroff(COLOR_PAIR(TRAP_COLOR));
                } else if (testch == enemiech) {
                    for (int i = 0; i < *num_enemies; i++) {
                        if (enemies[i].enemieX == ix && enemies[i].enemieY == iy && enemies[i].hp > 0) {
                            enemies[i].hp--;
                            if (enemies[i].hp == 0) {
                                attron(COLOR_PAIR(TRAP_COLOR));
                                mvaddch(ix, iy, '^' | A_BOLD);
                                attroff(COLOR_PAIR(TRAP_COLOR));
                                s->kills++;

                                // Atualize a posição do jogador, se necessário
                                if (ix == x && iy == y) {
                                    s->playerX = -1;
                                    s->playerY = -1;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

void update(STATE *st,int *num_enemies, ENEMIE *enemie){ // função que fornecerá à "do_movement_action" as informações acerca da próxima jogador do jogador
	int key = getch();
	

	switch (key)
	{
     case 't':
        // se o jogador não tiver a espada equipada, equipa a espada
        if (!st->sword) {
            st->sword = 1;
        } 
        // caso contrário, desequipa a espada
        else {
            st->sword = 0;
        }
        break;

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
	case ' ': attack(st,num_enemies,enemie); break;
	}
}

void spawn_enemie(ENEMIE *enemie, int * num_enemies, int y, int x){
  if (*num_enemies < 20){ // se o número de inimigos for menor do que 20 temos 30% de chance de ele aparecer(pode ser que o nº 0 seja gerado pela função rand que tem um alcance de 0 a 30) 
   ENEMIE new_enemie;
   char casa = ' ';
   int newX = (rand() % x-2);
   int newY = (rand() % y-2);
   char testch = mvinch(newX,newY) & A_CHARTEXT;
   if (testch == casa){
    new_enemie.enemieX = newX;
	new_enemie.enemieY = newY;
	new_enemie.hp = 1;
	enemie[*num_enemies] = new_enemie;
	(*num_enemies)++;
   }
  }
}

void draw_enemies(ENEMIE *enemies, int *num_enemies, STATE *s){
    for (int i = 0; i < *num_enemies; i++){
        ENEMIE *enemie = &enemies[i];
        move(enemie->enemieY, enemie->enemieX);
        if (mvinch(enemie->enemieY, enemie->enemieX) == ' '){
            attron(COLOR_PAIR(BACKGROUND));
            mvaddch(enemie->enemieY, enemie->enemieX, '&' | A_BOLD);
            attroff(COLOR_PAIR(BACKGROUND));
        }
    }
    move(s->playerY, s->playerX);
}

int main(){
	MAPA map;
	ENEMIE enemies[19];
	STATE st = {20, 20, 3, 0, 1, 0};
	WINDOW *wnd = initscr();
	int ncols, nrows, num_enemies = 0;
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
	init_pair(BULLET_ON, COLOR_YELLOW, COLOR_BLACK); // definição dos pares de cores utilizados
    init_pair(ENEMIE_COLOR, COLOR_MAGENTA, COLOR_BLACK);

	map.y = nrows;
	map.x = ncols;

	draw_map(&st, &map); // chamamento da função que desenha o mapa aletório

	while (1)
	{
      move(nrows - 1, 0);
      attron(COLOR_PAIR(1));
	  printw("(%d, %d) %d %d", st.playerX, st.playerY, ncols, nrows);
	  clrtoeol(); // limpa a linha atual para atualizar corretamente o scoreboard
	  printw("    Bullets: %d", st.bullets);
	  clrtoeol(); // limpa a linha atual para atualizar corretamente o scoreboard
	  printw("   HP: %d", (st.hp + 1));
	   clrtoeol(); // limpa a linha atual para atualizar corretamente o scoreboard
	  printw("   ENEMIES: %d", num_enemies);
       clrtoeol(); // limpa a linha atual para atualizar corretamente o scoreboard
      printw("   KILLS: %d", st.kills);
	  clrtoeol(); // limpa a linha atual para atualizar corretamente o scoreboard
      printw("   EQUIPPED:");
	  if (st.sword) {
      printw(" SWORD ");
     } else {
      printw(" GUN");
     }
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
			attroff(COLOR_PAIR(TRAP_COLOR));               // funções que desenham o jogador, mudando a sua cor consoante o hp
		}
        spawn_enemie(&enemies[num_enemies],&num_enemies,nrows,ncols);
        draw_enemies(&enemies[num_enemies],&num_enemies, &st);

		lights_off(&map); // função que apaga a luz da jogada anterior
		draw_light(&st,&map); // função que desenha a luz da nova jogada
		move(st.playerX, st.playerY);
		update(&st,&num_enemies,enemies); // chamamento da função update para atualizar o estado do jogador
	}

	return 0;
}
