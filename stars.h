/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2011 Jimmy Christensen <dusted@dusted.dk>             *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#ifndef STARS_H_INCLUDED
#define STARS_H_INCLUDED

#include <SDL/SDL.h>
#include "list.h"

struct star_s {
  int x, y, sx,sy;
  uint32_t color;
  int life;
  int fade;
};
typedef struct star_s star_t;

struct rocket_s {
  int x, y, sx, sy;
  int life;
  listItem* p;
};
typedef struct rocket_s rocket_t;

void initStars(SDL_Surface* screen);
void starField(SDL_Surface* screen, int move);
void fireWorks(SDL_Surface* screen);

#endif // STARS_H_INCLUDED
