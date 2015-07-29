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

int_fast8_t isAlpha(int_fast8_t r, int_fast8_t g, int_fast8_t b)
{
  return ( ( r==alphaR && g==alphaG && b==alphaB )?1:0);
}


//void plotPixel(SDL_Surface* img, int x, int y, uint16_t col)
void plotPixel(SDL_Surface* img, int x, int y, uint32_t col)
{
  //Bail if invalid position
  if(x < (HSCREENW-160) || x > (HSCREENW+159) || y < (HSCREENH-120) || y > (HSCREENH+119) )
  {
    return;
  }

  putpixel(img,x,y,col);

//  *(uint16_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;*/
}

//This is only used by software-scaler.
void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col)
{
  *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;
}


uint32_t freadPixel(SDL_Surface* img, int x, int y)
{
  return( *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+img->format->BytesPerPixel*x ) );
}

void debugPrintSurfaceInfo(SDL_Surface* s)
{
    printf( "Info for surface %p\n"
          " flags: 0x%X (%s) \n"
          " Pixelformat %p: \n"
          "  BitsPP: %i\n"
          "  BytesPP: %i\n"
          "  RMask: 0x%X\n"
          "  GMask: 0x%X\n"
          "  BMask: 0x%X\n"
          "  Colorkey: 0x%X\n"
          "  Alpha: 0x%X\n"
          " w: %i\n"
          " h: %i\n"
          " Pitch: %i\n"
          " Pixels: %p\n"
          " ClipRect:\n"
          "  x: %i\n"
          "  y: %i\n"
          "  w: %i\n"
          "  h: %i\n"
          " Refcount %i\n"
          "\n", s,
          s->flags, debugGetFlagInfo(s->flags),
          s->format,
          s->format->BitsPerPixel,
          s->format->BytesPerPixel,
          s->format->Rmask,
          s->format->Gmask,
          s->format->Bmask,
          s->format->colorkey,
          s->format->alpha,
          s->w,
          s->h,
          s->pitch,
          s->pixels,
          s->clip_rect.x,
          s->clip_rect.y,
          s->clip_rect.w,
          s->clip_rect.h,
          s->refcount );
}

char* debugGetFlagInfo( Uint32 flags )
{
  static char flagsStr[4096];
  flagsStr[0]='\0';

  if(flags & SDL_ANYFORMAT) sprintf(flagsStr, "%s", "SDL_ANYFORMAT,");
  if(flags & SDL_ASYNCBLIT) sprintf(flagsStr, "%s%s", flagsStr,"SDL_ASYNCBLIT,");
  if(flags & SDL_DOUBLEBUF) sprintf(flagsStr, "%s%s", flagsStr,"SDL_DOUBLEBUF,");
  if(flags & SDL_HWACCEL) sprintf(flagsStr, "%s%s", flagsStr,"SDL_HWACCEL,");
  if(flags & SDL_HWPALETTE) sprintf(flagsStr, "%s%s", flagsStr,"SDL_HWPALETTE,");
  if(flags & SDL_HWSURFACE) sprintf(flagsStr, "%s%s", flagsStr,"SDL_HWSURFACE,");
  if(flags & SDL_FULLSCREEN) sprintf(flagsStr, "%s%s", flagsStr,"SDL_FULLSCREEN,");
  if(flags & SDL_OPENGL) sprintf(flagsStr, "%s%s", flagsStr,"SDL_OPENGL,");
  if(flags & SDL_OPENGLBLIT) sprintf(flagsStr, "%s%s", flagsStr,"SDL_OPENGLBLIT,");
  if(flags & SDL_RESIZABLE) sprintf(flagsStr, "%s%s", flagsStr,"SDL_RESIZABLE,");
  if(flags & SDL_RLEACCEL) sprintf(flagsStr, "%s%s", flagsStr,"SDL_RLEACCEL,");
  if(flags & SDL_SRCALPHA) sprintf(flagsStr, "%s%s", flagsStr,"SDL_SRCALPHA,");
  if(flags & SDL_SRCCOLORKEY) sprintf(flagsStr, "%s%s", flagsStr,"SDL_SRCCOLORKEY,");
  if(flags & SDL_SWSURFACE) sprintf(flagsStr, "%s%s", flagsStr,"SDL_SWSURFACE,");
  if(flags & SDL_PREALLOC) sprintf(flagsStr, "%s%s", flagsStr,"SDL_PREALLOC,");

  return( (char*)&flagsStr );
}
