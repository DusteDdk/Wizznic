#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

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
#include <SDL/SDL_image.h>

typedef struct {
  SDL_Rect clip; //Position in the surface
  SDL_Surface* img; //The surface where this sprite resides, shared between sprites.
} spriteType;

typedef struct {
  int numFrames;
  int numTicks;
  int tick;
  int frame;
  spriteType** spr;
} aniType;

SDL_Surface* loadImg( const char* fileName ); //Ret 0 fail
spriteType* cutSprite(SDL_Surface* img, int x,int y, int w, int h); //Ret 0 fail
void drawSprite(SDL_Surface* scr, spriteType* spr, int x, int y);


aniType* mkAni(SDL_Surface*img, int w,int h, int ticksPerFrame);
void freeAni( aniType* ani );
void playAni(aniType* ani);
void drawAni(SDL_Surface* screen, aniType* ani, int x, int y);
void drawAniFrame(SDL_Surface* screen, aniType* ani, int x, int y, int frame);

#endif // SPRITE_H_INCLUDED
