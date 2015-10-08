#ifndef LEVELS_H_INCLUDED
#define LEVELS_H_INCLUDED

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
#include "stats.h"

struct levelInfo_s
{
  char* file;      //Level filename
  char* imgFile;   //Preview filename
  char* author;    //Author name
  char* levelName; //Level name
  char* tileBase;  //tile base name
  char* explBase;  //Base name for explotions
  char* wallBase;  //dir with wall.png and (optional) walls.png
  char* bgFile;    //backgroundFile
  char* musicFile; //Music file (set by pack.c)
  char* soundDir;  //Directory containing soundeffects
  char* fontName;  //Font
  char* cursorFile;//Cursor
  char* startImg; //img to show before game starts
  char* stopImg;  //img to show after level completed.

  list_t* teleList; //Initiated by mkLevelInfo
  list_t* switchList; //Initiated by mkLevelInfo

  int time; //Time in seconds to complete
  int brick_die_ticks;  //Ticks the bricks is to die
  int brickDieParticles; //Spawn particles when bricks die
  int_fast8_t showTelePath; //Highlight teleporter destinations
  int_fast8_t showSwitchPath; //Highlight switch destinations
  int_fast8_t completable;
};
typedef struct levelInfo_s levelInfo_t;


list_t* makeLevelList(const char* dir);
void makeUserLevelList();
void addUserLevel(const char* fn);
levelInfo_t* mkLevelInfo(const char* fileName);
void freeLevelInfo(levelInfo_t** p); //given a pointer to the pointer, so it can dereference it properly.
char* userLevelFile(int num);
int getNumUserLevels();


#endif // LEVELS_H_INCLUDED
