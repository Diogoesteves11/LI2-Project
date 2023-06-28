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
#define ENEMIE_COLOR 19
#define FLASHLIGHT 28
#define LOW_HP 29
#define EXPLOSION 30

#define N 19 
#define S 20
#define E 21
#define W 22
#define NW 23
#define SW 24
#define NE 25
#define SE 26
#define NO_DIRECTION 27

//#region FUNCTIONS
void move_monsters(STATE* st, MAPA* map, MONSTERS* monsters, int *num_enemies) {
    int playerX = st->playerX; // PlayerX
    int playerY = st->playerY; // PlayerY
    char monster = '&'; // Monster char
    int radius = 10; // radius of 10 blocks centered on the player
    
    for (int i = 0; i < (*num_enemies); i++) {
        int distacia_radius = abs (monsters[i].x-st->playerX) + abs(monsters[i].y - st->playerY);
        
        if(distacia_radius <= radius){ // if the monster is inside the radius centered on the player, the monster will chase the player
        int monsterX = monsters[i].x;
        int monsterY = monsters[i].y;
        
        int distanceX = abs(playerX - monsterX);
        int distanceY = abs(playerY - monsterY); // the monster calculates the shortest path to the player

        if (distanceX > distanceY) {
            if (playerX < monsterX) {
                monsterX--;
            } else if (playerX > monsterX) {
                monsterX++;
            }
        } else {
            if (playerY < monsterY) {
                monsterY--;
            } else if (playerY > monsterY) {
                monsterY++;
            }
        }

        if ((map->matrix[monsterX][monsterY] == '.' || map->matrix[monsterX][monsterY] == ' ') && (map->matrix[monsterX][monsterY] != '@' && (monsterX != playerX || monsterY != playerY) && map->matrix[monsterX][monsterY] != '&')) {
            map->matrix[monsters[i].x][monsters[i].y] = '.';
            monsters[i].x = monsterX;
            monsters[i].y = monsterY;
            map->matrix[monsterX][monsterY] = monster; // the monster makes a move if the next selected block is empty, otherwise the monster will stay still
        }

        }else{ // if the monster is outside the radius, it will move 1 block randomly in any of the 6 possible directions (N,S,E,W,NW,NE,SW,SE)
         int monsterX = monsters[i].x;
         int monsterY = monsters[i].y;

         int rand_direction = rand() % 8; 

         switch (rand_direction){
          case 0: monsterY--;break; 
          case 1: monsterY++; break; 
          case 2: monsterX++; break; 
          case 3: monsterX--; break;
          case 4: monsterX--; monsterY--; break; 
          case 5: monsterX--; monsterY++;break; 
          case 6: monsterX++; monsterY--;break; 
          case 7: monsterX++; monsterY++;break;
         }

        if ((map->matrix[monsterX][monsterY] == '.' || map->matrix[monsterX][monsterY] == ' ') && (map->matrix[monsterX][monsterY] != '@' && (monsterX != playerX || monsterY != playerY) && map->matrix[monsterX][monsterY] != '&')) {
            map->matrix[monsters[i].x][monsters[i].y] = '.';
            monsters[i].x = monsterX;
            monsters[i].y = monsterY;
            map->matrix[monsterX][monsterY] = monster; // the monster makes a move if the next selected block is empty, otherwise the monster will stay still
        }
        }
    }
}

void spawn_player(STATE *st, MAPA *map) {
    int x, y;
    do {
        x = (rand() % (map->x - 2)) + 1;
        y = (rand() % (map->y - 3)) + 2;
    } while (map->matrix[y][x] !=' '||
             map->matrix[y-1][x] == '#' || 
             map->matrix[y+1][x] == '#' || 
             map->matrix[y][x-1] == '#' || 
             map->matrix[y][x+1] == '#'); 

    st->playerX = y;
    st->playerY = x;

}

void draw_light(STATE *s,MAPA *map){ // this function is the implementation of the 'ray casting' algorithm 

int centerX = s->playerX;
int centerY = s->playerY;

char wall = '#';
char trap = 'x';
char heal = '+';
char bullet = '-';
char empty_block = ' ';
char enemy = '&';
char damage = '*';
char explosion = '^';

double delta = 0.05; 

    for (double angle = 0; angle < 2 * M_PI; angle += delta) {

        double dx = cos(angle);
        double dy = sin(angle);

        double x = centerX + 0.5; 
        double y = centerY + 0.5;

        while (x >= 0 && x < map->x && y >= 0 && y < map->y) {
            char testch = mvinch((int)y, (int)x) & A_CHARTEXT;
            
             if (testch == wall) {
                map-> matrix [(int)x][(int)y] = wall;
                attron(COLOR_PAIR(WALL_ILUMINATED));
                mvaddch(y, x, '#');
                attroff(COLOR_PAIR(WALL_ILUMINATED));
                break;
            }
            else if (testch == trap) {
                map-> matrix [(int)x][(int)y] = trap;
                attron(COLOR_PAIR(TRAP_COLOR));
                mvaddch(y, x, 'x' | A_BOLD);
                attroff(COLOR_PAIR(TRAP_COLOR));
                
            }
            else if (testch == heal) {
                map-> matrix [(int)x][(int)y] = heal;
                attron(COLOR_PAIR(HEAL_ON));
                mvaddch(y, x, '+' | A_BOLD);
                attroff(COLOR_PAIR(HEAL_ON));
                
            }
            else if (testch == bullet) {
                map-> matrix [(int)x][(int)y] = bullet;
                attron(COLOR_PAIR(BULLET_ON));
                mvaddch(y, x, '-' | A_BOLD);
                attroff(COLOR_PAIR(BULLET_ON));
                
            }
            else if (testch == empty_block) {
                map->matrix[(int)x][(int)y] = '.';
                attron(COLOR_PAIR(FLASHLIGHT)); 
                mvaddch(y, x, '.' | A_BOLD);
                attroff(COLOR_PAIR(FLASHLIGHT));
            }
            else if (testch == enemy){
               map-> matrix [(int)x][(int)y] = enemy;
               attron(COLOR_PAIR(ENEMIE_COLOR));
               mvaddch(y, x, '&' | A_BOLD);
               attroff(COLOR_PAIR(ENEMIE_COLOR));
             
            }
             else if (testch == damage){
              map->matrix [(int)x][(int)y] = damage;
             attron(COLOR_PAIR(TRAP_COLOR));
             mvaddch(y, x, '.' | A_BOLD);
             attroff(COLOR_PAIR(TRAP_COLOR));
             map->matrix[(int)x][(int)y] = ' ';
            }
            else if (testch == explosion){
             map->matrix [(int)x][(int)y] = explosion;
             attron(COLOR_PAIR(TRAP_COLOR));
             mvaddch(y, x, '^' | A_BOLD);
             attroff(COLOR_PAIR(TRAP_COLOR));
             map->matrix[(int)x][(int)y] = ' ';
           }
           
            x += dx;
            y += dy;
        }
    }
}

void lights_off(MAPA *map) { // this function sets all the map colors to black after each move, in order to clean the previous move from the screen. However, map items thar are useful to the player, as well as the walls already seen, aren´t changed
    char casa_iluminada = '.';
    char trap = 'x';
	  char enemy = '&';
    char damage = '*';
    char explosion = '^';
    char empty_block = ' ';

    
    for (int x = 1; x < map->x - 1; x++) {
        for (int y = 1; y < map->y - 1; y++) {
            char testch = map->matrix[x][y];
            if (testch == casa_iluminada) {
                map->matrix[x][y] = ' ';   
				        attron(COLOR_PAIR(BACKGROUND));         
				        mvaddch(y, x,map->matrix[x][y]);
			         	attroff(COLOR_PAIR(BACKGROUND));
            }
            else if (testch == trap) {
                map->matrix[x][y] = 'x';
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);
                mvaddch(y, x, 'x' | A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
			      else if (testch == enemy) {
                map->matrix[x][y] = '&';
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);
                mvaddch(y, x, enemy|A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
            else if (testch == damage){
                map->matrix[x][y] = '*';
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);
                mvaddch(y, x, map->matrix[x][y]|A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
            else if (testch == explosion){
               map->matrix[x][y] = '^';
                attron(COLOR_PAIR(BACKGROUND));
                attron(A_BOLD);
                mvaddch(y, x, map->matrix[x][y]|A_COLOR);
                attroff(COLOR_PAIR(BACKGROUND));
                attroff(A_BOLD);
            }
            else if(testch == empty_block) {
              map->matrix[x][y] = ' ';   
				      attron(COLOR_PAIR(BACKGROUND));         
				      mvaddch(y, x,map->matrix[x][y]);
			        attroff(COLOR_PAIR(BACKGROUND));
            }
        }
    }
    
}

void kill_monster(MONSTERS *monster, int index, MAPA *map){
  char empty_block = ' ',monsterch = '&';
  int newX = 0,newY = 0;
  do{
    newX = 1 + (rand()% map->x - 2);
    newY = 1 + (rand()% map->y - 2); 
  }while(map->matrix[newX][newY] != empty_block);
  monster[index].x = newX;
  monster[index].y = newY;
  monster[index].hp = 2;
  map->matrix[newX][newY] = monsterch; 
}

void draw_explosion (int x, int y, MAPA *map, MONSTERS *monsters, int *num_enemies){
  for (int ix = x-1; ix <= x +1; ix++){
    for (int iy= y - 1 ;iy<= y + 1; iy++){
      char testch = map->matrix[ix][iy];
      if (testch != '@' || testch != 'x' || testch != '&') {
        map->matrix [ix][iy] = '^';
      }
      else if (testch == 'x'){
        draw_explosion(ix,iy,map,monsters,num_enemies);
        map->matrix[ix][iy] = '^';
      }
      else if(testch == '&'){ // enemie
        for (int i = 0; i < (*num_enemies); i++){
          if(monsters[i].x == ix&& monsters[i].y == iy){
           kill_monster(ix,i,iy);
           map->matrix[ix][iy] = '^';
          } 
        }
      }
    }
  }
}

void do_movement_action(STATE *st, int dx, int dy,MAPA *map,MONSTERS *monsters, int *num_enemies){
	int nextX = st->playerX + dx;
	int nextY = st->playerY + dy;
	char trap = 'x';
	char wall = '#';
	char heal = '+';
	char bullet = '-';
	char enemich = '&';
	char testch = map->matrix[nextX][nextY];
	if (testch == wall) return; 
	else if (testch == trap) 
	{
			st->hp/=2;
      draw_explosion (nextX, nextY, map,monsters, num_enemies);
	}
	else if (testch == bullet) st->bullets ++;
	else if (testch == heal) st->hp += 2;
	else if (testch == enemich) return;
	mvaddch(st->playerY, st->playerX, ' ');
	st->playerX = nextX;
	st->playerY = nextY;
}

void attack(STATE *s, MAPA *map, MONSTERS *monster, int *direction, int *num_enemies){
  int x = s->playerX;
  int y = s->playerY;
  int dx = 0,dy = 0;
  
  char empty_block = ' ', casa_iluminada = '.', attack = '*', monsterch = '&', trap = 'x';
  if(s->sword){
    for (int ix = x-1; ix <= x+1; ix++){
     for (int iy = y-1; iy <= y+1; iy++){
      char testch = map->matrix[ix][iy];
      if(testch == empty_block || testch == casa_iluminada) map->matrix[ix][iy] = attack;
      else if (testch == monsterch){
        for (int i = 0; i < (*num_enemies); i++){
          if ((monster[i].x) == ix && (monster[i].y) == iy){
            if(monster[i].hp > 0){
              monster[i].hp--;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++;
               kill_monster(monster,i,map);
               map->matrix[ix][iy] = attack;
              }
              break;
            }
          }
        }
      }
    }
  }
 }
 else if (s->sword == 0){
  if(s->bullets > 0){
    switch (*direction){
    case N: {
     dx = 0;
     dy = -1;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < (*num_enemies); i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++;
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
     x += dx;
     y += dy;
     }
     break;
   } 
    case S:{
     dx = 0;
     dy = +1;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < (*num_enemies); i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++; 
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
      x += dx;
      y += dy;
     }
     break;
    }
    case W:{
     dx = -1;
     dy = 0;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < (*num_enemies); i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++; 
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
      x += dx;
      y += dy;
     }
     break;
    }
    case E: {
     dx = +1;
     dy = 0;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < (*num_enemies); i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++;
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
      x += dx;
      y += dy;
     }
     break;
    }
    case NW: {
     dx = -1;
     dy = -1;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < (*num_enemies); i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++;
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
      x += dx;
      y += dy;
     }
     break;
    }
    case NE: {
     dx = 1;
     dy = -1;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < (*num_enemies); i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++;
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
      x += dx;
      y += dy;
     }
     break;
    }
    case SW: {
     dx = -1;
     dy = +1;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < 15; i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp <= 0){
               s->kills++;
               s-> bullets++;
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
      x += dx;
      y += dy;
     }
     break;
    }
    case SE: {
     dx = 1;
     dy = +1;
     while (x < map->x && x > 0 && y < map->y && y > 0){
      char testch = map->matrix [x][y];
      if (testch == empty_block || testch == casa_iluminada){
        map->matrix[x][y] = attack;
      }
      else if (testch == monsterch){
         for (int i = 0; i < 15; i++){
          if ((monster[i].x) == x && (monster[i].y) == y){
            if(monster[i].hp > 0){
              monster[i].hp-=2;
              if(monster[i].hp == 0){
               s->kills++;
               s-> bullets++; 
               kill_monster(monster,i,map);
               map->matrix[x][y] = attack;
              }
              break;
            }
          }
      }
     }
     else if(testch == trap){
       draw_explosion(x,y,map,monster,num_enemies);
     }
      x += dx;
      y += dy;
     }
     break;
    }
  }
  s->bullets--;
 }
 
}
}
void update(STATE *st,MAPA *map,int *game_menu,MONSTERS *monster,int *direction, int *num_enemies, int *jump_on){
	int key = getch();

	switch (key)
	{
     case 't':
        if (st->sword == 0) {
            st->sword = 1;
        } 
        else if(st->sword == 1){
            st->sword = 0;
        }
        break;

	case KEY_A1:
	case '7':
		do_movement_action(st, -1, -1,map, monster, num_enemies);*direction = NW;
		break;
	case KEY_UP:
	case '8':
		do_movement_action(st, +0, -1,map, monster, num_enemies);*direction = N;
		break;
	case KEY_A3:
	case '9':
		do_movement_action(st, +1, -1,map, monster, num_enemies);*direction = NE;
		break;
	case KEY_LEFT:
	case '4':
		do_movement_action(st, -1, +0,map, monster, num_enemies);*direction = W;
		break;
	case KEY_B2:
	case '5': *direction = NO_DIRECTION;break; 
	case KEY_RIGHT:
	case '6':
		do_movement_action(st, +1, +0,map, monster, num_enemies);*direction = E;
		break;
	case KEY_C1:
	case '1':
		do_movement_action(st, -1, +1,map, monster, num_enemies);*direction = SW;
		break;
	case KEY_DOWN:
	case '2':
		do_movement_action(st, +0, +1,map, monster, num_enemies);*direction = S;
		break;
	case KEY_C3:
	case '3':
		do_movement_action(st, +1, +1,map, monster, num_enemies);*direction = SE;
		break;
	case 'q':
        *game_menu = 1;
		break;
	case 'w':
		do_movement_action(st, +0, -1,map, monster, num_enemies);*direction = N;
		break;
	case 'a':
		do_movement_action(st, -1, +0,map, monster, num_enemies);*direction = W;
		break;
	case 's':
		do_movement_action(st, +0, +1,map, monster, num_enemies);*direction = S;
		break;
	case 'd':
		do_movement_action(st, +1, +0,map, monster, num_enemies);*direction = E;
		break;
  case ' ': attack(st,map,monster,direction,num_enemies);break;
  case 'W':
  if(*jump_on){
   do_movement_action(st, +0, -2,map, monster, num_enemies);*direction = N;
	 break;
  }else {
    do_movement_action(st, +0, -1,map, monster, num_enemies);*direction = N;
	 break;
  }
  
	case 'A':
  if(*jump_on){
   do_movement_action(st, -2, +0,map, monster, num_enemies);*direction = W;
	 break;
  }else {
    do_movement_action(st, -1, +0,map, monster, num_enemies);*direction = W;
	 break;
  }
  
	case 'S':
  if(*jump_on){
   do_movement_action(st, +0, +2,map, monster, num_enemies);*direction = S;
		break;
  }else{
    do_movement_action(st, +0, +1,map, monster, num_enemies);*direction = S;
		break;
  }
	case 'D': 
  if(*jump_on){
    do_movement_action(st, +2, +0,map, monster, num_enemies);*direction = E;
		break;
  }else {
    do_movement_action(st, +1, +0,map, monster, num_enemies);*direction = E;
		break;
  }
  break;
	}
}

void show_menu() {
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);


    clear();
    
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
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
    mvprintw(start_y + 4, start_x, " 2. GAME DYNAMICS");
    mvprintw(start_y + 5, start_x, " 3. COMMAND LIST");
    mvprintw(start_y + 6, start_x, " 4. SETTINGS");
    mvprintw(start_y + 7, start_x, " 5. EXIT");
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(1));
    mvprintw(start_y + 8, start_x, "====================");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(start_y + 9, start_x, "  CHOOSE AN OPTION ");
    attroff(COLOR_PAIR(2));

    refresh();
}

void refresh_GAME_STATUS(MAPA *map){
    for (int ix = 0; ix <= map->x; ix++){
        for (int iy = 0; iy <= map->y; iy++){
         map->matrix[ix][iy] = '\0';
        }
    }
}


void spawn_monsters(MONSTERS *monster, MAPA *map, int *num_enemies){
  char empty_block = ' ';
  int ix,iy;
  for (int i = 0; i < (*num_enemies); i++){
   do{
   ix = 1 + (rand() % (map-> x)-2);
   iy = 1 + (rand() % (map-> y) - 2);
   }while(map->matrix[ix][iy]!= empty_block);
   map->matrix[ix][iy] = '&';
   monster[i].y = iy;
   monster[i].x = ix;
   monster[i].hp = 2;
  }
}

void enemy_attack(MONSTERS *monster, STATE *s, int *num_enemies){
  for(int i = 0; i < (*num_enemies); i++){
    for (int ix = ((monster[i].x)-1); ix <= (monster[i].x+1); ix ++){
     for(int iy = ((monster[i].y)-1); iy <= ((monster[i].y)+1); iy ++){
       if (ix == s->playerX && iy == s->playerY){
        s->hp --;
        break;
       }
     }
    }
  }
}
//#endregion

int main() {
  int map_layer = 1 + (rand() % 3); // RANDOM NUMBER OF FLOORS GENERATOR (FUTURE FEATURE)
  int in_game = 0;
  int in_submenu = 0;
  int in_game_dynamics = 0;
  int in_settings = 0;
  int num_enemies = 15;
  int jump_on = 0;
  int map_visibility = 0;
  MAPA map;

  while (1) {
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    start_color();
    init_pair(LIGHT, COLOR_WHITE, COLOR_BLACK);

    while (1) {
      show_menu(); 

      int choice = 0;
      choice = getch();
      clear();

      switch (choice) {
        case '1': {
          in_game = 1; 
          refresh_GAME_STATUS(&map);
          while (in_game) {
            int game_menu = 0;
            int plays = 0;
            STATE st = {20, 20, 3, 0, 1, 0};
            MONSTERS *monster;
            WINDOW *wnd = initscr();
            int ncols, nrows;
            int direction = NO_DIRECTION;
            getmaxyx(wnd, nrows, ncols);

            monster = malloc(sizeof(MONSTERS) * num_enemies);

            if(monster == NULL){
              in_game = 0;
              break;
            }

            srand48(time(NULL));
            start_color();

            cbreak();
            noecho();
            nonl();
            intrflush(stdscr, false);
            keypad(stdscr, true);

            init_pair(WALL_ILUMINATED, COLOR_CYAN, COLOR_BLUE);
            init_pair(LIGHT, COLOR_WHITE, COLOR_BLACK);
            init_pair(SCORE, COLOR_WHITE, COLOR_BLACK);
            init_pair(PLAYER, COLOR_BLACK, COLOR_GREEN);
            init_pair(BACKGROUND, COLOR_BLACK, COLOR_BLACK);
            init_pair(TRAP_COLOR, COLOR_RED, COLOR_BLACK);
            init_pair(MEDIUM_HP, COLOR_BLACK, COLOR_YELLOW);
            init_pair(HEAL_ON, COLOR_GREEN, COLOR_BLACK);
            init_pair(BULLET_ON, COLOR_YELLOW, COLOR_BLACK);
            init_pair(ENEMIE_COLOR, COLOR_BLACK, COLOR_MAGENTA);
            init_pair(FLASHLIGHT, COLOR_WHITE, COLOR_BLACK);
            init_pair(LOW_HP,COLOR_BLACK,COLOR_RED);
            init_pair(EXPLOSION,COLOR_RED,COLOR_RED);

            map.y = nrows;
            map.x = ncols;

            draw_map(&map,&map_visibility);
            spawn_player(&st, &map);
            spawn_monsters(monster,&map, &num_enemies);
           
            while (in_game) {
              move(nrows - 1, 0);
              attron(COLOR_PAIR(SCORE));
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
              clrtoeol();
              printw("   ENEMIES: %d", num_enemies);
              clrtoeol();
              printw("    JOGADAS: %d", plays);
              clrtoeol();
              if(jump_on){
                printw("   JUMP: ON");
              }else {
                printw("   JUMP: OFF");
              }
              if(map_visibility){
                printw("   MAP VISIBILITY: ON");
              }else {
                printw("   MAP VISIBILITY: OFF");
              }
              clrtoeol();
              mvprintw(nrows-1,ncols-19,"PRESS 'q' TO RETURN");
              attroff(COLOR_PAIR(SCORE));

              if (st.hp > 1) {
                map.matrix[st.playerX][st.playerY] = '@';
                attron(COLOR_PAIR(PLAYER));
                mvaddch(st.playerY, st.playerX, '@' | A_BOLD);
                attroff(COLOR_PAIR(PLAYER));
              } else if (st.hp == 1) {
                map.matrix[st.playerX][st.playerY] = '@';
                attron(COLOR_PAIR(MEDIUM_HP));
                mvaddch(st.playerY, st.playerX, '@' | A_BOLD);
                attroff(COLOR_PAIR(MEDIUM_HP));
              } else if (st.hp == 0){
                map.matrix[st.playerX][st.playerY] = '@';
                attron(COLOR_PAIR(LOW_HP));
                mvaddch(st.playerY, st.playerX, '@' | A_BOLD);
                attroff(COLOR_PAIR(LOW_HP));
              }else{
                in_game = 0;
                break;
              }
              
              lights_off(&map);
              draw_light(&st, &map);
              move(st.playerY, st.playerX);
              update(&st,&map, &game_menu,monster,&direction,&num_enemies,&jump_on);
              move_monsters(&st,&map,monster,&num_enemies);
              enemy_attack(monster,&st,&num_enemies);
              plays++;

              if (game_menu) {
                in_game = 0;
                
              }
            }
            free(monster);
          }
          break;
        }
        case '2':{
          in_game_dynamics = 1;
          while(in_game_dynamics){
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

            int starty = nrows/20;

            attron(COLOR_PAIR(LIGHT));
            mvprintw(starty,(ncols / 2)-9,"GAME DYNAMICS");
            mvprintw(starty+2,1,"MOVIMENT: ");
            mvprintw(starty+3,2,"1. The player only moves 1 block at a time, in any direction");
            mvprintw(starty+4,2,"2. By pressing the number 5, the player does not move and the game refreshes");
            mvprintw(starty+5,2,"3. The player can only move towards an empty block, or a item such as bullets, traps or heals");
            mvprintw(starty+6,2,"4. The enemies move randomly when they're out of the 10 block radius. Once they're incide the radius, they move towards the player");
            mvprintw(starty+7,2,"5. The player has a 'JUMP' function that can be enabled in settings. This allows the player to move 2 blocks at a time if the next block is valid");

            mvprintw(starty+9,1,"LIGHTING SYSTEM: ");
            mvprintw(starty+10,2,"1.The player has a flashlight that iluminates the map in any direction. The light is dynamic, witch means that it adapts to the map.");

            mvprintw(starty+13,1,"COMBAT: ");
            mvprintw(starty+14,2,"1. Each hit given by the monster to the player reduces the player's hp by 1");
            mvprintw(starty+15,2,"2. Each player starts with 4 hp and 0 bullets.");
            mvprintw(starty+16,2,"3. The hp and bullets are rechargeable, by picking up the loot present on the map");
            mvprintw(starty+17,2,"4. Each hit given by the sword reduces 1hp to the enemy");
            mvprintw(starty+18,2,"5. Each hit given by the gun kills the monster");
            mvprintw(starty+19,2,"6. If the player steps on a trap, the player's hp is divided by 2 until the player is 1hp");
            mvprintw(starty+20,2,"7. The sowrd has a 1 block range while the gun has a unlimited range, enless the bullet hits a wall");
            mvprintw(starty+21,2,"8. The number of enemies is variable, however, when a enemy is killed, another one spawns");
            mvprintw(starty+22,2,"9. The enemies have a 1 block range");
 
           mvprintw(starty+25,1,"OHER:");
           mvprintw(starty+26,2,"1.The game has a difficulty system that can be change in 'GAME SETTINGS'");
           mvprintw(starty+27,2,"2.Inside the game settings menu, the user can change game dynamics such has the map visibility, the difficulty and the jump");
           mvprintw(nrows-2,1,"PRESS 'q' TO RETURN");
           attroff(COLOR_PAIR(LIGHT));

           int key = getch();
           switch(key){
            case 'q': in_game_dynamics = 0;break;
           }
          }
          break;
        }
        case '3':{
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

            int startx = ncols /20;
            int starty = nrows/20;

            attron(COLOR_PAIR(LIGHT)); 
            mvprintw(starty, (ncols - 7) / 2, "CONTROL");

            
            mvprintw(starty + 2, startx, "t       -> switch gun");
            mvprintw(starty + 3, startx, "w/8     -> move UP");
            mvprintw(starty + 4, startx, "s/2     -> move DOWN");
            mvprintw(starty + 5, startx, "a/4     -> move LEFT");
            mvprintw(starty + 6, startx, "d/6     -> move RIGHT");
            mvprintw(starty + 7, startx, "7       -> move UPPER LEFT");
            mvprintw(starty + 8, startx, "9       -> move UPPER RIGHT");
            mvprintw(starty + 9, startx, "1       -> move DOWN LEFT");
            mvprintw(starty + 10, startx, "3       -> move DOWN RIGHT");
            mvprintw(starty + 11, startx, "SPACE   -> USE WEAPON");
            mvprintw(starty + 12, startx, "5       -> NO MOVEMENT");
            mvprintw(starty + 13, startx, "SHIFT   -> JUMP");


            mvprintw(nrows-2,1,"PRESS 'q' TO RETURN");
            attroff(COLOR_PAIR(LIGHT));


            int key = getch();
            switch(key){
             case 'q': in_submenu = 0;break;
            }
           }
           break;
        }
        case '4': {
         in_settings = 1;
         while(in_settings){
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

            int startx = ncols /20;
            int starty = nrows/20;

            attron(COLOR_PAIR(LIGHT)); 
            mvprintw(starty, (ncols - 7) / 2, "CHOOSE DIFFICULTY");
            if (num_enemies == 5){
             mvprintw(starty + 2, startx, "1.[x] EASY: 5 enemies");
             mvprintw(starty + 3, startx, "2.[ ] MEDIUM: 15 enemies");
             mvprintw(starty + 4, startx, "3.[ ] HARD: 20 enemies");
             mvprintw(starty + 5, startx, "4.[ ] IMPOSSIBLE: 30 enemies");
             clrtoeol();
            }
            else if (num_enemies == 15){
             mvprintw(starty + 2, startx, "1.[ ] EASY: 5 enemies");
             mvprintw(starty + 3, startx, "2.[x] MEDIUM: 15 enemies");
             mvprintw(starty + 4, startx, "3.[ ] HARD: 20 enemies");
             mvprintw(starty + 5, startx, "4.[ ] IMPOSSIBLE: 30 enemies");
             clrtoeol();
            }
            else if (num_enemies == 20){
             mvprintw(starty + 2, startx, "1.[ ] EASY: 5 enemies");
             mvprintw(starty + 3, startx, "2.[ ] MEDIUM: 15 enemies");
             mvprintw(starty + 4, startx, "3.[x] HARD: 20 enemies");
             mvprintw(starty + 5, startx, "4.[ ] IMPOSSIBLE: 30 enemies");
             clrtoeol();
            }
            else if (num_enemies == 30){
             mvprintw(starty + 2, startx, "1.[ ] EASY: 5 enemies");
             mvprintw(starty + 3, startx, "2.[ ] MEDIUM: 15 enemies");
             mvprintw(starty + 4, startx, "3.[ ] HARD: 20 enemies");
             mvprintw(starty + 5, startx, "4.[x] IMPOSSIBLE: 30 enemies");
             clrtoeol();
            }
            if(jump_on){
             mvprintw(starty + 7, startx, "5.Activate Jump: ON");
             clrtoeol();
            }else {
              mvprintw(starty + 7, startx, "5.Activate Jump: OFF");
              clrtoeol();
            }
            if(map_visibility){
             mvprintw(starty + 8, startx, "6.Map visibility: ON");
             clrtoeol();
            }else {
              mvprintw(starty + 8, startx, "6.Map visibility: OFF");
              clrtoeol();
            }
            mvprintw(nrows-2,1,"PRESS 'q' TO RETURN");
            attroff(COLOR_PAIR(LIGHT));

            int key = getch();
            switch(key){
             case 'q': in_settings = 0;break;
             case '1': num_enemies = 5;refresh();break;
             case '2': num_enemies = 15;refresh();break;
             case '3': num_enemies = 20;refresh();break;
             case '4': num_enemies = 30;refresh();break;
             case '5': {
              if(jump_on){
                jump_on = 0;refresh();break;
              }else {
                jump_on = 1;refresh();break;
              }
             }
             case '6': {
              if(map_visibility){
                map_visibility = 0;refresh();break;
              }else {
                map_visibility = 1;refresh();break;
              }
             }
            }
         }
         break;
        }
        case '5': {
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
