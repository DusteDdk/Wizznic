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

#ifndef WAVEIMG
#define WAVEIMG

#include <SDL/SDL.h>
struct wavingImage_s
{
  SDL_Surface *screen, *img;
  int x,y,rotations,amount,speed;
  float privRotAmount;
};
typedef struct wavingImage_s wavingImage_t;
//void waveImg(SDL_Surface* screen, SDL_Surface* img, int x, int y,int rots, int amount, int speed);
void waveImg(wavingImage_t* wi);

void setWaving(wavingImage_t* wi, SDL_Surface* screen, SDL_Surface* img, int x, int y, int rots, int amount, int speed);

#endif
