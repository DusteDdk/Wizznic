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

#include "cursor.h"
#include "settings.h"

void setCursor(cursorType* c, int x, int y)
{
  c->dx = x;
  c->dy = y;
  c->x = x;
  c->y = y;
  c->moving=0;
  c->lock=0;
  c->px = c->dx*brickSize+boardOffsetX-4;
  c->py = c->dy*brickSize+boardOffsetY-4;
}

void updateCursor(cursorType* c)
{
  //printf("Cur: (%i,%i) Dst: (%i,%i) Lock: %i\n",c->x,c->y,c->dx,c->dy, c->lock);
  int destX = c->dx*brickSize+boardOffsetX-4;
  int destY = c->dy*brickSize+boardOffsetY-4;

  if(c->px != destX || c->py != destY)
    c->moving=1;
    else
    c->moving=0;

  //If Cursor is locked to a brick, let the board-code set cursor position
  if(c->lock) return;

  if(c->px > destX)
  {
    c->px -= CURSORMOVESPEED;
    if(c->px < destX)
      c->px=destX;
  } else if(c->px < destX)
  {
    c->px += CURSORMOVESPEED;
    if(c->px > destX)
      c->px=destX;
  } else {
    c->x=c->dx;
  }

  if(c->py > destY)
  {
    c->py -= CURSORMOVESPEED;
    if(c->py < destY)
      c->py=destY;
  } else if(c->py < destY)
  {
    c->py += CURSORMOVESPEED;
    if(c->py > destY)
      c->py=destY;
  } else {
    c->y=c->dy;
  }

}

void moveCursor(cursorType* c, int dirX, int dirY, int limit)
{

  int dx = c->dx + dirX;
  int dy = c->dy + dirY;

  if(dx < 0 || dx+1 > FIELDSIZE  || dy < 0 || dy+1 > FIELDSIZE)
  {
    return;
  }

  if(c->x == c->dx || !limit)
  {
    c->dx += dirX;
  }

  if(c->y == c->dy || !limit)
  {
    c->dy += dirY;
  }

}

void initCursor(cursorType* c)
{
  c->x = 5;
  c->y = 5;
  c->dx=c->x;
  c->dy=c->y;
  c->px=HSCREENW;
  c->py=HSCREENH;
}
