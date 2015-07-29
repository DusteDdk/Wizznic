#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

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
#include "ticks.h"

#define NUMFONTS 4

#define GAMEFONTMEDIUM 3
#define GAMEFONTSMALL 2
#define FONTMEDIUM 1
#define FONTSMALL 0


void txtInit(); //load menu charactersets
void txtLoadGameCharSet(const char* font);
void txtFreeGameCharSet();
void txtWrite( SDL_Surface* scr,int font, const char* txt, int x, int y);
void txtWriteCenter( SDL_Surface* scr,int fontNum, const char* txt, int x, int y);
void txtWave( SDL_Surface* scr, int fontNum, const char* txt, int x, int y, float* rot);
int* getCharSize(int font);
SDL_Rect* getTxtBox();

#endif // TEXT_H_INCLUDED
