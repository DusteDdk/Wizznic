#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED

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

#include "list/list.h"
#include "text.h"
#include <SDL/SDL.h>

//Highscore entries (also used for in-game stats)
struct hsEntry_s
{
  //Player Name, only used in pack-wide higshcore
  char name[12];

  //packWide: level user entered highscore
  int levelNum;

  int score;

  //PrPack: total playtime before score achieved (fails also counted)
  //PrLevel:time used to solve level
  int time;

  //PrPack:Lives Spent
  //PrLevel:Number of moves.
  int moves;

  //PrLevel: Combos
  int combos;
};
typedef struct hsEntry_s hsEntry_t;


//Stats for the package
struct stats_s
{
  char* hsFn; //Stats file to read from
  int progress; //Progress in this pack
  list_t* levelStats; //list of hsEntries, one for each level, with "best" stats
  list_t* packHsTable; //pack-wide highscores
  hsEntry_t* cl; //ptr to current levelStats
};
typedef struct stats_s stats_t;

struct statsFileHeader_s {
  int hsFileVersion;  //Version of file
  int progress;       //Highest level player has yet achieved
  int numLevelEntries;//Number of level-best hsEntries to read
  int numHsEntries;   //Number of packWide hsEntries to read
};
typedef struct statsFileHeader_s statsFileHeader_t;

void statsSetLevel(int l); //Simply set the cl ptr.
stats_t* stats(); //Return ptr to stats_t
void statsInit(); //Sets some ptrs 0, that's all.
void statsLoad(); //Tries to load stats from score/fn.bin
void statsSubmitBest();
void statsDrawHs( SDL_Surface* screen );
int statsIsHighScore();
void statsSaveHighScore();
void statsSave();

void statsReset();
void packUnlinkHsFile();

void statsUpload(int level, int time, int moves, int combos, int score, const char* action, int ignoreIfOnline, int* retVal);

#endif // STATS_H_INCLUDED
