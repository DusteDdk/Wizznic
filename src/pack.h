#ifndef PACK_H_INCLUDED
#define PACK_H_INCLUDED

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

#include <SDL/SDL.h>

#include "list/list.h"
#include "sprite.h"
#include "levels.h"

#define PACK_IS_NOT_DLC 0
#define PACK_IS_DLC 1

struct packInfo_s {
  char* name; //Name user sees
  char* author;
  char* comment;
  char* path; //Path to pack
  SDL_Surface* icon;
  list_t* levels; //LevelInfo*'s
  int numLevels;
  int hasFinishedImg;
  int lives;
  int isDLC;
};
typedef struct packInfo_s packInfoType;

struct packState_s {
  int numPacks; //Number of packs
  int selected; //Currently selected
  list_t* packs; //PackInfoType*s
  packInfoType* cp; //Current pack
  packInfoType* dlc; //The "DLC" entry, on the pack list.
  SDL_Surface* packBoxImg; //Graphics for the box
  SDL_Surface* finishedImg;; //Image shown when cp is completed (0 = not been loaded)
  spriteType* packBoxSpr[5];
};
typedef struct packState_s packStateType;

typedef struct {
  int from,to;
  char* song;
} playListItem;

void packInit(); //Generate pack list
void packSet(int packNum); //Set selected pack, default to wizznic if out of range.
void packSetByPath(const char* dir); //Search through packs to find dir, if none found, default to 0.
packStateType* packState(); //Return ptr to packstate.
const char* packGetFile(const char* path,const char* fn); //Return filename for file in currently selected pack, or from wizznic if not found there.

levelInfo_t* levelInfo(int num);
int getNumLevels();

void packFreeGfx(); //Free the images and sprites used to show packs.

void drawPackBox(SDL_Surface* screen, int posx, int posy, int packNum);

int isFile(const char* fileName);
int isDir(const char* dirName);

//Returns the number of the added pack.
int packAdd(const char* packDir, int isDLC);

#endif // PACK_H_INCLUDED
