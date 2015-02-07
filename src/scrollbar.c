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
#include "defs.h"
#include "sprite.h"
#include "scrollbar.h"
#include "pointer.h"
#include "ticks.h"

static int sbCur;

static int dirCntDwn;

static spriteType* scrollBarGfx[4];

void scrollBarInit()
{
  sbCur=0;
  dirCntDwn=0;
  scrollBarGfx[0] = cutSprite(loadImg(DATADIR"data/menu/scrollbar.png"),0,0,19,186);
  scrollBarGfx[1] = cutSprite(scrollBarGfx[0]->img,0,186,17,3);
  scrollBarGfx[2] = cutSprite(scrollBarGfx[0]->img,0,186+3,11,8);
  scrollBarGfx[3] = cutSprite(scrollBarGfx[0]->img,0,186+3+8,11,8);
}

//Returns current position if a change happened, or -1 if nothing happened.
//size is the number of items in the list, current is the currently selected item.
int scrollBar(SDL_Surface* screen, int x, int y, float size, int current, int chdir)
{
  SDL_Rect r;
  int retVal=-1;

  if( chdir < 0 )
  {
    dirCntDwn = -120;
  } else if( chdir >  0 )
  {
    dirCntDwn = 120;
  }

  drawSprite(screen, scrollBarGfx[0], x, y);

  //Draw indicator
  drawSprite(screen, scrollBarGfx[1], x+1,y+18+round(147.0/size*(float)current) );

  //Check for mouse hover over buttons
  r.x = x;
  r.w = r.x+18;
  r.y = y;
  r.h = r.y+18;
  if( (isPointerInBox(&r) && current > 0) || dirCntDwn < 0)
  {
    drawSprite(screen, scrollBarGfx[2], r.x+4, r.y+6 );
    if(isPointerClicked())
    {
      retVal=current-1;
    }
  }
  //Check for mouse hover over buttons
  r.y = y+167;
  r.h = r.y+18;
  if( (isPointerInBox(&r) && current < size) || dirCntDwn > 0 )
  {
    drawSprite(screen, scrollBarGfx[3], r.x+4, r.y+5 );
    if(isPointerClicked())
    {
      retVal=current+1;
    }
  }

  r.y = y+16;
  r.h = r.y+152;
  if( isPointerInBox(&r) && getInpPointerState()->isDown )
  {
    retVal=(int)round( size/150.0 * (float)(getInpPointerState()->vpY-(r.y)));
  }

  if(dirCntDwn < 0 )
  {
    dirCntDwn += getTicks();
    if( dirCntDwn > 0 )
    {
      dirCntDwn=0;
    }
  } else if(dirCntDwn > 0)
  {
    dirCntDwn -= getTicks();
    if(dirCntDwn < 0 )
    {
      dirCntDwn=0;
    }
  }

  return(retVal);
}
