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

#include "pixel.h"
#include "math.h"
#include "defs.h"

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
 int bpp = surface->format->BytesPerPixel;
 // Here p is the address to the pixel we want to set
 Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

 switch(bpp)
 {
  case 1:
   *p = pixel;
   break;
  case 2:
   *(Uint16 *)p = pixel;
   break;
  case 3:
   if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
   {
    p[0] = (pixel >> 16) & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = pixel & 0xff;
   } else {
    p[0] = pixel & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = (pixel >> 16) & 0xff;
   }
   break;
  case 4:
   *(Uint32 *)p = pixel;
   break;
 }
}



//inline void plotPixel(SDL_Surface* img, int x, int y, uint16_t col)
inline void plotPixel(SDL_Surface* img, int x, int y, uint32_t col)
{
  //Bail if invalid position
  if(x < 0 || x > (SCREENW-1) || y < 0 || y > (SCREENH-1) )
  {
    return;
  }

  putpixel(img,x,y,col);

//  *(uint16_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;*/
}

inline void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col)
{
  *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;
}


inline uint32_t freadPixel(SDL_Surface* img, int x, int y)
{
 return( *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+img->format->BytesPerPixel*x ) );
}

