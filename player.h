#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "stats.h"

struct player_s {
  int gameStarted; // Is game started
  int inEditor; //If the player is in editor
  int level;
  int lives;
  char* levelFile;

  hsEntry_t hsEntry; //Per-level highscore stats
  hsEntry_t campStats; //Pr-campain stats (updated by stats-best)
};
typedef struct player_s player_t;

void initPlayer();
player_t* player();

int getBest(int field, char* file); //Returns the field in the level db.

#define BESTTIME 0
#define BESTSCORE 1
#define BESTCOMBO 2
#define BESTMOVES 3
#define TIMESPLAYED 4

#endif // PLAYER_H_INCLUDED
