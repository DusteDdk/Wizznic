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

#include <math.h>

#include "waveimg.h"
#include "pixel.h"
#include "ticks.h"
#include "settings.h"

void setWaving(wavingImage_t* wi, SDL_Surface* screen, SDL_Surface* img, int x, int y, int rots, int amount, int speed)
{
  wi->screen=screen;
  wi->img=img;
  wi->x=x;
  wi->y=y;
  wi->rotations=rots;
  wi->amount=amount;
  wi->speed=speed;
}

//void waveImg(SDL_Surface* screen, SDL_Surface* img, int xx, int yy, int rots, int amount, int speed)
void waveImg(wavingImage_t* wi)
{

  int x, y; //In the source image
  int nx, ny; //new x/y value for px
  uint32_t col; //Color of pixel
  int r,g,b;


  float pxInc = (6.28318531/wi->img->w)*wi->rotations;

  float yInc;

  wi->privRotAmount -=(float)getTicks()/wi->speed;

  for(x=0; x < wi->img->w; x++)
  {
    yInc = cos(wi->privRotAmount+x*pxInc)*wi->amount;

    for(y=0; y < wi->img->h; y++)
    {
      col = freadPixel(wi->img, x, y);

      //Do expensive colorkeying
      r = ((col & wi->img->format->Rmask) >> wi->img->format->Rshift);
      g = ((col & wi->img->format->Gmask) >> wi->img->format->Gshift);
      b = ((col & wi->img->format->Bmask) >> wi->img->format->Bshift);

      //These values... I finally get it, green have bit more than blue and red, and it's ALL GREEN, largest values in 6 bits is 63, since blue have a bit less, largest value is 31., for 24 bpp, they all have 3 bytes.
      if( (setting()->bpp==2 && !(r==0 && g==63 && b==31)) || (setting()->bpp==3 && !(r==0x0 && g==0xff && b==0xff) ) )
      {
        nx = x;
        ny = y+yInc;
        plotPixel(wi->screen, nx+wi->x,ny+wi->y, col);
      }
    }
  }
}

