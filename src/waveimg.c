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

  wi->useOverlay=0;
  wi->jumpPos=0;
}

//void waveImg(SDL_Surface* screen, SDL_Surface* img, int xx, int yy, int rots, int amount, int speed)
void waveImg(wavingImage_t* wi)
{

  int x, y,ox=0; //In the source image
  int nx, ny; //new x/y value for px
  uint32_t col; //Color of pixel
  int r,g,b;

  float pxInc = (6.28318531/wi->img->w)*wi->rotations;

  float yInc;

  wi->privRotAmount -=(float)getTicks()/wi->speed;

  //If we use overlay, move it
  if( wi->useOverlay )
  {
    wi->overlayPos += wi->overlaySpeed;
    wi->jumpPos = wi->overlay->h/4 + cos(wi->privRotAmount/2)*wi->overlay->h/4;

  }

  for(x=0; x < wi->img->w; x++)
  {
    yInc = round( cos(wi->privRotAmount+x*pxInc)*wi->amount );

    if(wi->useOverlay)
    {
      ox=(wi->overlayPos-x)%wi->overlay->w;
    }

    for(y=0; y < wi->img->h; y++)
    {
      col = freadPixel(wi->img, x, y);

      //Do expensive colorkeying
      r = ((col & wi->img->format->Rmask) >> wi->img->format->Rshift);
      g = ((col & wi->img->format->Gmask) >> wi->img->format->Gshift);
      b = ((col & wi->img->format->Bmask) >> wi->img->format->Bshift);

      if( !isAlpha(r,g,b) )
      {
        nx = x;
        ny = y+yInc;
        //Cheap colorkey, basically, if the green component of the mask is = then we use the pixel.
        if( wi->useOverlay && ( (freadPixel(wi->mask, x, y) & wi->mask->format->Gmask) >> wi->mask->format->Gshift == 0 ) )
        {
            col = freadPixel(wi->overlay, ox, wi->jumpPos+y);
        }

        plotPixel(wi->screen, nx+wi->x,ny+wi->y, col);
      }
    }
  }
}

