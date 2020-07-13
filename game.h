#ifndef GAME_H
#define GAME_H

#include "gba.h"


typedef struct enemy {
  int row;
  int col;
  int prev_row;
  int direction;
  int edge;
} ENEMY;

typedef enum {
  START,
  PLAY,
  TROPHY,
  WIN,
  LOSE,
} GBAState;

struct player {
  int row;
  int col;
  int prev_row;
  int prev_col;   
  int edge;    
  int speed;
};

#define ENEMY_NUM 9

void init_enemies(void);
void draw_enemies(void);
int collision_system(void);
void init_player(void);
void draw_player(u32);
int get_trophy_yet(void);
#endif
