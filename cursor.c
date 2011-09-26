#include "cursor.h"
#include "settings.h"

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

int moveCursor(cursorType* c, int dirX, int dirY, int limit)
{

  int dx = c->dx + dirX;
  int dy = c->dy + dirY;

  if(dx < 0 || dx+1 > FIELDSIZE  || dy < 0 || dy+1 > FIELDSIZE) return(0);

  if(c->x == c->dx || !limit)
  {
    c->dx += dirX;
  }

  if(c->y == c->dy || !limit)
  {
    c->dy += dirY;
  }

  return(1);
}

void initCursor(cursorType* c)
{
  c->x = 5;
  c->y = 5;
  c->dx=c->x;
  c->dy=c->y;
  c->px=160;
  c->py=120;
}
