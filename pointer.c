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
 * along with Wizznic.  If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

#include "pointer.h"
#include "ticks.h"
#include "defs.h"
#include "sprite.h"
#include "pixel.h"

static inpPointerState_t inpPointer;
static SDL_Surface* ptrBackImg;
static SDL_Rect ptrBackRect;

inline inpPointerState_t* getInpPointerState()
{
  return(&inpPointer);
}

void initPointer(SDL_Surface* screen)
{
  //Load the "escape" overlay image
  ptrBackImg = loadImg(DATADIR"data/ptr-back.png");
  if( !ptrBackImg )
    printf("Couldn't open %s\n",DATADIR"data/ptr-back.png");

  ptrBackRect.x=0;
  ptrBackRect.y=0;
  ptrBackRect.w=ptrBackImg->w;
  ptrBackRect.h=ptrBackImg->h;

  memset( &inpPointer, 0, sizeof(inpPointerState_t) );
  inpPointer.timeSinceMoved=POINTER_SHOW_TIMEOUT;

  //The color white
  inpPointer.colWhite = SDL_MapRGBA( screen->format, 255,255,255,255 );

}

inline void drawPointer(SDL_Surface* screen)
{
  if( inpPointer.timeSinceMoved < POINTER_SHOW_TIMEOUT  )
  {
    //Update pointer-holddown time
    if(inpPointer.isDown)
      inpPointer.downTime += getTicks();

    if( inpPointer.escEnable )
    {
      inpPointer.escEnable=0;
      SDL_BlitSurface( ptrBackImg, NULL, screen,NULL);
    }

    plotPixel(screen, inpPointer.vpX, inpPointer.vpY-2, inpPointer.colWhite );
    plotPixel(screen, inpPointer.vpX, inpPointer.vpY+2, inpPointer.colWhite );

    plotPixel(screen, inpPointer.vpX-2, inpPointer.vpY, inpPointer.colWhite );
    plotPixel(screen, inpPointer.vpX+2, inpPointer.vpY, inpPointer.colWhite );

  }
}

//The mousepointer is only "clicked" the first iteration that it is held down (when downTime is still 0)
//So, if the downtime is 0, then there is a chance that it is being hold down.
int_fast8_t isPointerClicked()
{
  if( inpPointer.downTime==0 )
  {
    return(inpPointer.isDown);
  }
  return(0);
}

int_fast8_t isPointerInBox( SDL_Rect* r )
{
  if( r->x < inpPointer.vpX && r->w > inpPointer.vpX && r->y < inpPointer.vpY && r->h > inpPointer.vpY )
  {
    inpPointer.hitABox=1;
    return(1);
  }
  return(0);
}

int_fast8_t isBoxClicked( SDL_Rect* r )
{
  if( isPointerClicked() )
  {
    return( isPointerInBox(r));
  }
  return(0);
}

int_fast8_t isPointerEscapeClicked()
{
  if( inpPointer.escEnable && isBoxClicked( &ptrBackRect ) )
  {
    inpPointer.hitABox=0;
    return(1);
  }
  return(0);
}

int_fast8_t isAnyBoxHit()
{
  return(inpPointer.hitABox);
}

