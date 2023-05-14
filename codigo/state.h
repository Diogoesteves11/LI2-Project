#ifndef ___STATE_H___
#define ___STATE_H___

#define MAX 2000

typedef struct mapa{
	int x;
	int y;
	char matriz [MAX] [MAX]; // primeira matriz é a das colunas(x)
} MAPA;

typedef struct state {
	int playerX;
	int playerY;
	int hp;
	int bullets;
	int sword;
} STATE;

typedef struct enemie {
   int enemieID;	
   int enemieX;
   int enemieY;
   int enemiehp;
   int speed;
} ENEMIE;  

#endif
