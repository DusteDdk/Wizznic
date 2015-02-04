#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

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
#include "defs.h"
#include "board.h"
#include "cursor.h"
#include "sprite.h"
#include "list/list.h"
#include "particles.h"
#include "levels.h"
#include "teleport.h"

#define TELEPATHNUMCOL 32
struct boardGraphics_t
{
  SDL_Surface* boardImg;
  SDL_Surface* background;

  SDL_Surface* tileImg;
  SDL_Surface* curImg;
  SDL_Surface* wallImg;//Single wall
  SDL_Surface* wallsImg;//If theme uses extra wall tiles.
  SDL_Surface* explImg[BRICKSEND]; //Brick destroy animations, either one pr tile, or all defaults to 0.
  SDL_Surface* aniImg[NUMTILES];
  SDL_Surface* countDownImg;
  spriteType* curSpr[2];
  spriteType* tiles[NUMTILES];

  aniType* brickExpl[BRICKSEND];
  aniType* tileAni[NUMTILES];

  spriteType* walls[13];
  spriteType* countDownSpr[4]; //Countdown graphics 3,2,1,start

  //Teleport path animation
  int teleColorIndex;
  uint32_t teleColorTable[TELEPATHNUMCOL];
};

int initDraw(levelInfo_t* li, SDL_Surface* screen);
void cleanUpDraw();
void draw(cursorType* cur, playField* pf, SDL_Surface* screen);
void drawTelePath( SDL_Surface* screen, telePort_t* tp, int animate );
void drawPath( SDL_Surface* screen, int sx, int sy, int dx, int dy, int animate );
void drawAllTelePaths( SDL_Surface* screen,list_t* tp );
void drawShowCountDown(SDL_Surface* screen, int i);
struct boardGraphics_t* stealGfxPtr();
//For editor
void drawBrick(SDL_Surface* screen, int brickNum, int x, int y);

//For image dumper
#ifndef GP2X
void drawDisableCursor();
#endif

#endif // DRAW_H_INCLUDED
