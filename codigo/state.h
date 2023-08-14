#ifndef ___STATE_H___
#define ___STATE_H___

#define MAX 2000

typedef struct mapa{
	int x;
	int y;
	char matrix [MAX] [MAX];
} MAPA;

typedef struct state {
	int playerX;
	int playerY;
	int hp;
	int bullets;
	int sword;
	int kills;
	int room;
} STATE;

typedef struct monsters
{
    int y;
    int x;
    int hp;
}MONSTERS;

#endif
