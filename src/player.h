#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2015 Jimmy Christensen <dusted@dusted.dk>             *
 * Wizznic is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * Wizznic is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Wizznic.  If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

#include "stats.h"

struct player_s {
  int gameStarted; // Is game started
  int inEditor; //If the player is in editor
  int level;
  int lives;
  char* levelFile;

  int timeouts; //Number of times the player has run out of time during the same level

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
