/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2012 Jimmy Christensen <dusted@dusted.dk>             *
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

static int_fast8_t alphaR,alphaG,alphaB;

void setAlphaCol( int bpp )
{
  if( bpp==2 )
  {
    alphaR=0x00;
    alphaG=0x3f;
    alphaB=0x1f;
  } else if( bpp==3 )
  {
    alphaR=0x00;
    alphaG=0xff;
    alphaB=0xff;
  } else {
    printf("setAlphaCol: Error, bpp=%i we don't support that, we will now crash.\n",bpp);
  }
}

inline int_fast8_t isAlpha(int_fast8_t r, int_fast8_t g, int_fast8_t b)
{
  return ( ( r==alphaR && g==alphaG && b==alphaB )?1:0);
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

//This is only used by software-scaler.
inline void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col)
{
  *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;
}


inline uint32_t freadPixel(SDL_Surface* img, int x, int y)
{
 return( *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+img->format->BytesPerPixel*x ) );
}

