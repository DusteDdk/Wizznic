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

#include "board.h"

#include <stdio.h>
#include "ticks.h"
#include "strings.h"
#include "teleport.h"
#include "particles.h"
#include "draw.h"
#include "input.h"


static int isWall(playField* pf, int x, int y)
{
  if(x < 0) return(0);
  if(x+1 > FIELDSIZE) return(0);
  if(y < 0) return(0);
  if(y+1 > FIELDSIZE) return(0);

  if(!pf->board[x][y]) return(0);
  if(pf->board[x][y]->type==STDWALL) return(1);
  if(pf->board[x][y]->type==GLUE) return(1);
  if(pf->board[x][y]->type==ONEWAYLEFT) return(1);
  if(pf->board[x][y]->type==ONEWAYRIGHT) return(1);

  return(0);
}

void setWallType(playField* pf, int x, int y)
{
  //see if has neighbours:
  //Top-Left
  if( !isWall(pf, x-1,y) && !isWall( pf, x, y-1) && isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 0;
  }
  //Top
  if( isWall(pf, x-1,y) && !isWall( pf, x, y-1) && isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 1;
  }
  //Top-Right
  if( isWall(pf, x-1,y) && !isWall( pf, x, y-1) && !isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 2;
  }
  //Left
  if( !isWall(pf, x-1,y) && isWall( pf, x, y-1) && isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 3;
  }
  //Middle
  if( isWall(pf, x-1,y) && isWall( pf, x, y-1) && isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 4;
  }
  //Right
  if( isWall(pf, x-1,y) && isWall( pf, x, y-1) && !isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 5;
  }
  //LeftBot
  if( !isWall(pf, x-1,y) && isWall( pf, x, y-1) && isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 6;
  }
  //Bot
  if( isWall(pf, x-1,y) && isWall( pf, x, y-1) && isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 7;
  }
  //RightBot
  if( isWall(pf, x-1,y) && isWall( pf, x, y-1) && !isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 8;
  }
  //Left,Top,Right
  if( !isWall(pf, x-1,y) && !isWall( pf, x, y-1) && !isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 9;
  }
  //Left, top, bot
  if( !isWall(pf, x-1,y) && !isWall( pf, x, y-1) && isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 10;
  }
  //Top, right, bot
  if( isWall(pf, x-1,y) && !isWall( pf, x, y-1) && !isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 11;
  }
  //Left, bot, right
  if( !isWall(pf, x-1,y) && isWall( pf, x, y-1) && !isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 12;
  }
  //Vert
  if( !isWall(pf, x-1,y) && isWall( pf, x, y-1) && !isWall(pf, x+1,y) && isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 13;
  }
  //Horiz
  if( isWall(pf, x-1,y) && !isWall( pf, x, y-1) && isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall = 14;
  }

  //Middle - clear
  if( !isWall(pf, x-1,y) && !isWall( pf, x, y-1) && !isWall(pf, x+1,y) && !isWall(pf,x,y+1) )
  {
    pf->board[x][y]->wall=15;
  }
}

void boardSetWalls(playField* pf)
{
  int x,y;
  for(x=0; x < FIELDSIZE; x++)
  {
    for(y=0; y < FIELDSIZE; y++)
    {
      if(pf->board[x][y] && ( pf->board[x][y]->type==STDWALL || pf->board[x][y]->type==GLUE || pf->board[x][y]->type==ONEWAYLEFT || pf->board[x][y]->type==ONEWAYRIGHT ) )
      {
        setWallType(pf,x,y);
      }
    }
  }
}

int loadField(playField* pf, const char* file)
{
  FILE *f = fopen(file, "r");
  if(!f)
  {
    printf("%s %i Board: couldn't open '%s'\n",__FILE__,__LINE__,file);
  }

  char temp[32];
  int type=0;

  //For atoi hack
  temp[2] = '\0';

  //Fast forward to the interesting section.
  while( fgets( temp,32, f) )
  {
    stripNewLine( temp);
    if(strcmp("[data]", temp)==0)
    {
      break;
    }
  }
  //For checking that level var correctly loaded
  //Read each byte into the field
  int c=0,x=0,y=0;
  do {
    c = fgetc(f);
    temp[0]=c;

    if(c=='\n')
    {
        y++;
        x=0;
        if(y == FIELDSIZE)
        {
          break;
        }
    } else if(c=='\r')
    {
      //Ignore windows wanting to run on a typewriter.
    } else {
      if(x==FIELDSIZE) { return(0); } //Error loading level, bail before accesing invalid index.

      temp[1]=fgetc(f);
      type=atoi(temp);

      if(type !=0)
      {
        pf->board[x][y] = malloc( sizeof( brickType ) );
        pf->board[x][y]->type = type;
        pf->board[x][y]->pxx = x*brickSize+boardOffsetX;
        pf->board[x][y]->pxy = y*brickSize+boardOffsetY;
        pf->board[x][y]->dir=0;
        pf->board[x][y]->checked=0;
        pf->board[x][y]->curLock=0;
        pf->board[x][y]->sx = x;
        pf->board[x][y]->sy = y;
        pf->board[x][y]->dx = x;
        pf->board[x][y]->dy = y;
        pf->board[x][y]->tl  = MOVERCOUNTDOWN;
        pf->board[x][y]->moveXspeed = 0;
        pf->board[x][y]->moveYspeed = 0;

        pf->board[x][y]->dmx = 0;
      } else {
        pf->board[x][y] = 0;
      }
      x++;
    }

  } while( c != EOF );

  //Close the file
  fclose(f);

  pf->movingList = initList();
  pf->removeList = initList();

  pf->blocker = malloc(sizeof(brickType));
  pf->blocker->type=RESERVED;

  //Figure out which tile to use for each wall (int 6)
  boardSetWalls(pf);

  return(1);
}

void freeField(playField* pf)
{
  int x,y;
  for(y=0; y < FIELDSIZE; y++)
  {
    for(x=0; x < FIELDSIZE; x++)
    {
      if(pf->board[x][y])
      {
        //We don't want to free the blocker more than once.
        if(pf->board[x][y]->type!=RESERVED)
        {
          free(pf->board[x][y]);
        }
        pf->board[x][y]=0;
      }
    }
  }
  free(pf->blocker);
  //Clear the lists
  freeList(pf->movingList);
  freeList(pf->removeList);

}

int moveBrick(playField* pf, int x, int y, int dirx, int diry, int block, int speed)
{
  //Destination x
  int dx = x+dirx;
  int dy = y+diry;

  //Trying to move out of field?
  if( dx < 0 || dx == FIELDSIZE ) return(0);
  if( dy < 0 || dy == FIELDSIZE ) return(0);


  if(!pf->board[x][y]) return(0);

  pf->board[x][y]->dmx=0; //Just always reset it.

  //OneWay or glue below?
  if(y+1<FIELDSIZE)
  {
    if(isOneWay(pf->board[x][y+1]))
    {
      if(pf->board[x][y+1]->type==ONEWAYLEFT && dirx==DIRRIGHT) return(0);
      if(pf->board[x][y+1]->type==ONEWAYRIGHT && dirx==DIRLEFT) return(0);
    } else if(pf->board[x][y+1] &&pf->board[x][y+1]->type==GLUE)
    {
      return(0);
    }
  }

  //If destination is empty
  if( !pf->board[dx][dy] || (block==NOBLOCK && (!pf->board[dx][dy] || pf->board[dx][dy]->type==RESERVED)) )
  {
      //Set destination
      pf->board[x][y]->dx=dx;
      pf->board[x][y]->dy=dy;

      //Set source
      pf->board[x][y]->sx=x;
      pf->board[x][y]->sy=y;

      //Set moving speed
      pf->board[x][y]->moveXspeed=speed*dirx;
      pf->board[x][y]->moveYspeed=speed*diry;

      //add to moving
      listAddData(pf->movingList, (void*)pf->board[x][y]);

      pf->board[dx][dy]=pf->blocker;
      pf->board[x][y]=pf->blocker;

    return(1);
  }

  return(0);
}

//Move a brick instantly to teleports dest
void telePortBrick(playField* pf,telePort_t* t,cursorType* cur)
{
  psysSet_t ps; //Particle system for particle effect
  brickType* b = pf->board[t->sx][t->sy];

  //Setup particle system
  ps.layer=PSYS_LAYER_TOP;
  ps.fade=0;
  ps.gravity=0;
  ps.bounce=0;
  ps.srcImg=0;


  //Spawn systems in source
  ps.layer=PSYS_LAYER_TOP;
  ps.x=b->pxx+stealGfxPtr()->tiles[b->type-1]->clip.w/2;
  ps.y=b->pxy+stealGfxPtr()->tiles[b->type-1]->clip.h/2;
  ps.vel=100; // +/- in each dir
  ps.life=350;
  ps.lifeVar=10;
  ps.color=PARTICLECOLORRANDOM;
  ps.numParticles=60;
  spawnParticleSystem(&ps);
  ps.vel=50; // +/- in each dir
  ps.life=200;
  ps.lifeVar=10;
  ps.color=0xFFFF;
  ps.numParticles=30;
  spawnParticleSystem(&ps);


  //Move brick to dest
  pf->board[t->dx][t->dy]=b;
  pf->board[t->sx][t->sy]=0;

  //Set pixel position
  b->pxx=boardOffsetX+20*t->dx;
  b->pxy=boardOffsetY+20*t->dy;

  b->dx=t->dx;
  b->dy=t->dy;

  b->sx=b->dx;
  b->sy=b->dy;

  //Spawn system in dest
  ps.layer=PSYS_LAYER_TOP;
  ps.x=b->pxx+stealGfxPtr()->tiles[b->type-1]->clip.w/2;
  ps.y=b->pxy+stealGfxPtr()->tiles[b->type-1]->clip.h/2;
  ps.vel=100; // +/- in each dir
  ps.life=300;
  ps.lifeVar=10;
  ps.color=PARTICLECOLORRANDOM;
  ps.numParticles=60;
  spawnParticleSystem(&ps);
  ps.vel=50; // +/- in each dir
  ps.life=200;
  ps.lifeVar=10;
  ps.color=0xFFFF;
  ps.numParticles=30;
  spawnParticleSystem(&ps);

  //We detach mouse because it makes no sense to have it locked on,
  //since the destination might be too far away, and the user won't want
  //the brick to move towards it's source again.
  if( getInpPointerState()->isDown )
  {
    b->curLock=0;
  } else   //Move cursor?
  if( cur->lock && cur->x == t->sx && cur->y == t->sy )
  {
    b->curLock=1;
    //Move cursor
    cur->px=b->pxx-4;
    cur->py=b->pxy-4;
    cur->x=b->dx;
    cur->y=b->dy;
    cur->dx=b->dx;
    cur->dy=b->dy;
  }

  //Play sound
  sndPlayOnce(SND_TELEPORTED, b->pxx);

}

void doTelePort(playField* pf,cursorType* cur)
{
  listItem* li=pf->levelInfo->teleList;

  while( (li=li->next) )
  {
    telePort_t* t = (telePort_t*)li->data;

    //Check if theres something in src, and that dst is free
    if( pf->board[t->sx][t->sy] && !pf->board[t->dx][t->dy] )
    {
      //Is it a brick that's in sx ?
      if(isBrick(pf->board[t->sx][t->sy]))
      {
        telePortBrick(pf, t,cur);
      }
    }
  }
}

//BUG: This function will move a mover which is getting atop or below another.
//Don't have movers that can touch.
static int vertMover(playField* pf,int x, int y, int dir)
{
  //Outside bounds
  if(y+dir < 0 || y+dir == FIELDSIZE) return(0);

  //Abort if it's not a brick, or a mover.
  if(!isBrick(pf->board[x][y]) && !isMover(pf->board[x][y]) ) return(0);

  //Found a space
  if( !pf->board[x][y+dir] )
  {
     moveBrick(pf,x,y,0,dir, NOBLOCK, VERTMOVERSPEED);
     return(1);
  } else if(vertMover(pf, x, y+dir, dir))
  {
    moveBrick(pf,x,y,0,dir, NOBLOCK, VERTMOVERSPEED);
    return(1);
  }

  return(0);
}

static int bricksOnTop(playField* pf, int x, int y)
{
  int num=0;
  while(1)
  {
    y--;
    if(y < 0) return(num);
    if(pf->board[x][y] && isBrick(pf->board[x][y]))
      num++;
      else
    return(num);
  }

}

//Returns 0 if couldn't move. Returns 1 if could.
static int horizMover(playField* pf, int x, int y, int dir)
{
  //Out of bounds
  if(x+dir<FIELDSIZE && x+dir>-1)
  {
    //Can it move to the side?
    if( !pf->board[x+dir][y] )
    {
      //Move
      moveBrick(pf,x,y,dir,0, DOBLOCK, HORIZMOVERSPEED);

      //Move the ones on top (if possible)
      int stop=0;
      while( !stop )
      {

        if(x+dir < FIELDSIZE && x+dir > -1)
        {
          y--;
          if( y > -1 && pf->board[x][y] && isBrick(pf->board[x][y]) )
          {
            //Can we move that brick one to the dir?
            if( !pf->board[x+dir][y] )
            {
              moveBrick(pf,x,y,dir,0, DOBLOCK, HORIZMOVERSPEED);
            }
          } else { stop=1; }

        } else { stop=1; }

      }

    } else { //Can't move that direction
      return(0);
    }
  } else { //No more level, can't move that direction.
    return(0);
  }

  //Moved that way.
  return(1);
}

void simField(playField* pf, cursorType* cur)
{
  int x,y;

  //Update moving bricks
  listItem* li=pf->movingList;
  brickType* b;
  x=0;
  while( (li = li->next) )
  {
    x++;
   // printf("Bricks in list: %i\n",x);
    b=(brickType*)li->data;

    //Do we need to move it?
    int deltaX= (b->dx*brickSize+boardOffsetX) - b->pxx ;
    int deltaY= (b->dy*brickSize+boardOffsetY) - b->pxy ;

    if(deltaX || deltaY )
    {
      //Doing this lock to only move one dir at a time
      if( deltaX )
      {
        b->pxx += b->moveXspeed;

        if(b->moveXspeed < 0 && b->pxx < (b->dx*brickSize+boardOffsetX) )
          b->pxx = (b->dx*brickSize+boardOffsetX);

        if(b->moveXspeed > 0 && b->pxx > (b->dx*brickSize+boardOffsetX) )
          b->pxx = (b->dx*brickSize+boardOffsetX);

      }
      if( deltaY)
      {
        b->pxy += b->moveYspeed;
      }
      //}
      //Is cursor attached?
      if(cur->lock && b->curLock)
      {
        //Update cursor
        cur->px=b->pxx-4;
        cur->py=b->pxy-4;
      }
    //  printf("Dx:%i X:%i Dy:%i Y:%i\n",deltaX,b->pxx, deltaY, b->pxy);
    } else { //Not moving anymore, put back down on board

      if(cur->lock && b->curLock)
      {
        cur->x=b->dx;
        cur->y=b->dy;
        cur->dx=b->dx;
        cur->dy=b->dy;
      }

      //Set moving speed 0
      b->moveXspeed=0;
      b->moveYspeed=0;

      //Put it down:
      pf->board[ b->dx ][ b->dy ] = b;


      //Clear source
      pf->board[ b->sx ][ b->sy ] = 0;

      //Set source pos = destination pos
      b->sx=b->dx;
      b->sy=b->dy;

      //Remove brick from moving list
      listRemoveItem( pf->movingList, li );
    }
  }

  //May I be forgiven for I do not know better.
  brickType* hack;

  //Run teleport rules first
  doTelePort(pf,cur);

  //Static bricks
  for(y=FIELDSIZE-1; y > -1; y--)
  {
    for(x=0; x < FIELDSIZE; x++)
    {
      if( pf->board[x][y] && !pf->board[x][y]->checked)
      {
        pf->board[x][y]->checked=1;

        //Is it a brick
        if( isBrick(pf->board[x][y]) )
        {
          //Cursor locked on it?
          if( cur->lock && cur->x == x && cur->y == y )
          {
            pf->board[x][y]->curLock=1;
          } else {
            pf->board[x][y]->curLock=0;
          }

          //Things that happens below it
          if(y+1 < FIELDSIZE)
          {

            //Falling?
            if( !pf->board[x][y+1] )
            {
               //Move down
              moveBrick(pf, x, y, 0, DIRDOWN, DOBLOCK, FALLINGSPEED);
            } else //Laying on a reserved brick might mean that it should be attached to a mover.
            {
              if( pf->board[x][y+1]->type == RESERVED) //Magnet to mover
              {
                // Warning: Confuzing and weird stuff below
                b=findMoving(pf,x,y+1);
                if(b)
                {
                  //Recurse down to see if there is a mover below.
                  li=pf->movingList;
                  while( (li=li->next) )
                  {
                    if(b)
                    {
                      if(b->type==MOVERHORIZ)
                      {
                        if(b->sx!=pf->board[x][y]->dx) { break; }
                        //Magnet onto brick
                        hack=pf->board[x][y];
                        if(moveBrick(pf,x,y,(b->dx-b->sx),0, DOBLOCK, HORIZMOVERSPEED))
                        {
                          hack->pxx=b->pxx;
                          hack->moveXspeed=b->moveXspeed;
                          hack->pxx -= hack->moveXspeed; //To sync with the rest of the bricks. (they allready moved this frame)
                          break;
                        }
                      } else if(b->type==MOVERVERT)
                      {

                        //Only magnet if it's moving down (if it's moving up, it will eventually hit the resting brick)
                        if(b->sy < b->dy)
                        {
                          //Fetch the original underlying brick.
                          b=findMoving(pf, x,y+1);
                          hack=pf->board[x][y];
                          if(moveBrick(pf,x,y,0,(b->dy-b->sy),NOBLOCK, VERTMOVERSPEED))
                          {
                            hack->pxy=b->pxy-20;
                            hack->moveYspeed=b->moveYspeed;
                            break;
                          }
                        }
                      }

                      b=findMoving(pf,x,b->dy+1);
                    } else {
                      break;
                    }

                  } //Loop through list
                }
              } //Resting on a reserved
            } //Not free
          }
        }

        //Is it a mover
        if(isMover(pf->board[x][y]))
        {
          //Horiz mover?
          if(pf->board[x][y]->type == MOVERHORIZ)
          {
            //Moving right?
            if(pf->board[x][y]->dir)
            {
              if(!horizMover(pf, x,y, 1))
              {
                pf->board[x][y]->tl -=getTicks();
                if(pf->board[x][y]->tl < 1)
                {
                  pf->board[x][y]->dir = 0;
                  pf->board[x][y]->tl  = MOVERCOUNTDOWN;
                }
              }
            } else { //Moving left
              if(!horizMover(pf, x,y, -1))
              {
                pf->board[x][y]->tl -=getTicks();
                if(pf->board[x][y]->tl < 1)
                {
                  pf->board[x][y]->dir = 1;
                  pf->board[x][y]->tl  = MOVERCOUNTDOWN;
                }
              }
            }
          } else if(pf->board[x][y]->type== MOVERVERT)
          {
            //Vertical mover
            if(pf->board[x][y]->dir)
            {
              //Moving up
              if(!vertMover(pf,x,y,-1))
              {
                pf->board[x][y]->tl -= getTicks();
                if(pf->board[x][y]->tl < 1)
                {
                  pf->board[x][y]->dir = 0;
                  pf->board[x][y]->tl  = MOVERCOUNTDOWN;
                }
              }
            } else {
              int numOnTop = bricksOnTop(pf,x,y);
              if(!vertMover(pf,x,y-numOnTop, 1))
              {
                pf->board[x][y]->tl -= getTicks();
                if(pf->board[x][y]->tl < 1)
                {
                  pf->board[x][y]->dir = 1;
                  pf->board[x][y]->tl  = MOVERCOUNTDOWN;
                }
              }
            }
          }
        }
        else if(isOneWay(pf->board[x][y])) //One way floor
        {
          //Try to move the block on top of it, if it's a brick
          if(y>0 && pf->board[x][y-1] && isBrick(pf->board[x][y-1]))
          {
            if(pf->board[x][y]->type==ONEWAYLEFT)
            {
               if(moveBrick(pf, x,y-1, DIRLEFT, 0, DOBLOCK, ONEWAYSPEED))
               {
                 sndPlayOnce(SND_ONEWAY_MOVE, boardOffsetX+x*brickSize);
               }
            }
            if(pf->board[x][y]->type==ONEWAYRIGHT)
            {
              if(moveBrick(pf, x,y-1, DIRRIGHT, 0, DOBLOCK, ONEWAYSPEED))
              {
                 sndPlayOnce(SND_ONEWAY_MOVE, boardOffsetX+x*brickSize);
              }
            }

          }
        }

      }
    }
  }

  for(y=FIELDSIZE-1; y > -1; y--)
  {
    for(x=0; x < FIELDSIZE; x++)
    {
      if(pf->board[x][y])
      {
        //Unmark "checked" status.
        pf->board[x][y]->checked=0;

        //Do delayed x movement
        if( isBrick(pf->board[x][y]) && pf->board[x][y]->dmx )
        {
          b=pf->board[x][y];
          curMoveBrick(pf, b, b->dmx);
          b->dmx=0;
        }

      }
    }
  }
}

inline int isBrick(brickType* b)
{
  if( b->type < BRICKSBEGIN || b->type > BRICKSEND ) return(0);
  return(1);
}

int doRules(playField* pf)
{
  int x,y;
  int removed=0;
  int bricksLeft=0;

  //Used to count how many of each type there is (simple detection of unsolvable lvl)
  int typeLeft[BRICKSEND]; //Bricktype offset = -1
  for(x=0;x<BRICKSEND;x++) typeLeft[x]=0; //Set to 0 to start with.

  //Count moving bricks
  listItem* li=pf->movingList;
  brickType* b;
  while( (li = li->next) )
  {
    if( isBrick((brickType*)li->data) )
      bricksLeft++;
  }

  for(y=FIELDSIZE-1; y > -1; y--)
  {
    for(x=0; x < FIELDSIZE; x++)
    {
      if(pf->board[x][y] && isBrick(pf->board[x][y]))
      {
        //Bricks on board
        bricksLeft++;

        //Tell this type exists on the board (for detecting if it's still solvable)
        typeLeft[pf->board[x][y]->type-1]++;


        //Is brick, it will only remove itself, this avoids multiple entries of the same brick.
        if(!isBrickFalling(pf,pf->board[x][y]))
        {
          //Detect touching bricks.

          //On top
          if(y > 0 && pf->board[x][y-1] && pf->board[x][y-1]->type == pf->board[x][y]->type && !isBrickFalling(pf,pf->board[x][y-1]) )
          {
            pf->board[x][y]->dir=1;
            listAddData(pf->removeList, (void*)pf->board[x][y]);
            //pf->board[x][y]=0;
         } else
          //Below
          if(y+1 < FIELDSIZE && pf->board[x][y+1] && pf->board[x][y+1]->type == pf->board[x][y]->type && !isBrickFalling(pf,pf->board[x][y+1]) )
          {
            pf->board[x][y]->dir=1;
            listAddData(pf->removeList, (void*)pf->board[x][y]);
            //pf->board[x][y]=0;
          } else
          //Left
          if(x > 0 && pf->board[x-1][y] && pf->board[x-1][y]->type == pf->board[x][y]->type && !isBrickFalling(pf,pf->board[x-1][y]) )
          {
            pf->board[x][y]->dir=1;
            listAddData(pf->removeList, (void*)pf->board[x][y]);
            //pf->board[x][y]=0;
          } else
          //Right
          if(x+1 < FIELDSIZE && pf->board[x+1][y] && pf->board[x+1][y]->type == pf->board[x][y]->type && !isBrickFalling(pf,pf->board[x+1][y]))
          {
            pf->board[x][y]->dir=1;
            listAddData(pf->removeList, (void*)pf->board[x][y]);
            //pf->board[x][y]=0;
          }

        } //A brick
      } //Not falling
    }
  }
  //Remove ones that need removed
  li=pf->removeList;
  while( (li = li->next) )
  {
    //Count dying bricks as alive untill they are really removed
    bricksLeft++;
    b=(brickType*)li->data;
    if(b->dir)
    {
      sndPlayOnce(SND_BRICKBREAK,b->pxx);
      b->dir=0;
      //Set die time left
      b->tl=pf->levelInfo->brick_die_ticks;
      //Reserve, to prevent bricks from falling into the animation
      pf->board[b->dx][b->dy]=pf->blocker;
    } else {
      b->tl -= getTicks();

      if(b->tl < 1)
      {
        removed++;
        //Unreserve
        pf->board[b->dx][b->dy]=0;
        //Dealloc the brick
        free(b);
        //Remove from list
        listRemoveItem(pf->removeList, li);
      }
    }
  }

  //Check for solvability, if no bricks were removed, no bricks are moving, and no bricks are to be removed
  //resuing x as counter.
  if(!removed && !pf->removeList->next && !pf->movingList->next)
  {
    for(x=0;x<BRICKSEND;x++)
    {
      if(typeLeft[x]==1)
      {
        return(UNSOLVABLE);
      }
    }
  }

  if(removed)
    return(removed);

  if(!bricksLeft)
    return(NOBRICKSLEFT);

  return(0);
}

int isBrickFalling(playField* pf, brickType* b)
{
  if(b->sy+1 < FIELDSIZE)
  {
    if( !pf->board[b->sx][b->sy+1] )
    {
      return(1);
    }
    //Check if there is a reserved brick below it, that is moving the same way
    if( pf->board[b->sx][b->sy+1] && pf->board[b->sx][b->sy+1]->type == RESERVED )
    {
      brickType* bb = findMoving(pf,b->sx,b->sy+1);
      if( bb && (bb==b || (bb->pxx != b->pxx || bb->moveXspeed != b->moveXspeed || bb->moveYspeed != b->moveYspeed)) )
      {
        return(1);
      }
    }
  }
  return(0);
}


int isMover(brickType* b)
{
  if(!b) return(0);
  if(b->type==MOVERHORIZ||b->type==MOVERVERT) return(1);
  return(0);
}

int isOneWay(brickType* b)
{
  if(!b) return(0);
  if(b->type==ONEWAYLEFT||b->type==ONEWAYRIGHT) return(1);
  return(0);
}

//Return the brick moving into or out of fieldx/y
brickType* findMoving(playField* pf, int x, int y)
{
  brickType* br;
  //Bail if position is invalid
  if(x>=FIELDSIZE || y>=FIELDSIZE) return(0);

  //Bail if it's not a reserved brick
  if(!pf->board[x][y] || pf->board[x][y]->type!=RESERVED) return(0);

  listItem* li=pf->movingList;
  while( (li = li->next) )
  {
    br=(brickType*)li->data;
    if( (br->sx == x && br->sy==y) || (br->dx==x && br->dy==y) )
    {
      return(br);
    }

  }
  return(0);
}

brickType* brickUnderCursor(playField* pf, int x, int y)
{
  brickType* b=findMoving(pf,x,y);

  //If it's not moving, they maybe it's standing still
  if( !b && pf->board[x][y] )
    b=pf->board[x][y];

  if( b && isBrick(b) )
    return(b);

  return(0);
}

int curMoveBrick(playField *pf, brickType *b, int dir)
{
  //We can't move the brick, if it is falling.
  if( isBrickFalling(pf, b) ) return(0);

  //Move brick that is not moving<.
  if(b->moveXspeed==0 && b->moveYspeed==0)
  {
    //First, check that it's a brick, and not a reserved.
    if(pf->board[b->sx][b->sy] && isBrick( pf->board[b->sx][b->sy]) )
    {
      if(moveBrick( pf, b->sx, b->sy, dir, 0, DOBLOCK, CURSORMOVESPEED))
      {
        return(1);
      }
    }
  } else {
    if( ((b->moveXspeed < 0 && dir > 0) || (b->moveXspeed > 0 && dir <0)) || b->moveYspeed )
    {
      if(!b->dmx)
      {
        b->dmx = dir;
        return(1);
      }
    }
  }
  return(0);
}
