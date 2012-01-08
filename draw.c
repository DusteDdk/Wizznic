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

#include "draw.h"
#include "pack.h"
#include "defs.h"
#include "settings.h"
#include "input.h"

struct boardGraphics_t graphics;

int initDraw(levelInfo_t* li, SDL_Surface* screen)
{
  char tempStr[512];
  int i,x,y;

  //Background image
  sprintf(tempStr,"%s.png",li->bgFile);
  graphics.boardImg = loadImg( packGetFile("themes",tempStr) );

  if(!graphics.boardImg)
  {
    cleanUpDraw();
    printf("Couldn't load board file:'%s'\n", packGetFile("themes",tempStr) );
    return(0);
  }

  //Tileset
  sprintf(tempStr, "%s.png", li->tileFile);
  graphics.tileImg = loadImg( packGetFile("themes",tempStr) );
  if(!graphics.tileImg)
  {
    cleanUpDraw();
    printf("Couldn't load tile file:'%s'\n", packGetFile("themes",tempStr) );
    return(0);
  }

  //Extra walls, if they exist, if they don't, default to tile 6 (from 0) in tiles.
  sprintf(tempStr, "%s-walls.png", li->tileFile);
  graphics.wallImg = loadImg( packGetFile("themes",tempStr) );
  if(!graphics.wallImg) printf("Optional GFX missing: '%s'\n", packGetFile("themes",tempStr) );
  int r,c; //rows, column, sprite index
  i=0;
  for(r=0; r < 5; r++)
  {
    for(c=0; c < 3; c++)
    {
      if(graphics.wallImg)
      {
        //Cut out from sheet
        x=c*20;
        y=r*20;
        graphics.walls[i] = cutSprite(graphics.wallImg, x,y, 20, 20);
      } else {
        //Default to tile 15 in tileset
        graphics.walls[i] = cutSprite(graphics.tileImg, 20*15, 0, 20, 20);
      }
      i++;
    }
  }
  //Above loop leaves when i==15.
  //Middle-free is 15 = default tile index in image is 15 (starting from 0).
  graphics.walls[15] = cutSprite(graphics.tileImg, 20*15, 0, 20, 20);

  //Explosions, reuse R as frame number index
  for(i=0; i < BRICKSEND; i++)
  {
    //Open explosion
    sprintf(tempStr, "%s-expl%i.png", li->tileFile, i);
    graphics.explImg[i] = loadImg( packGetFile("themes",tempStr) );

    if(!graphics.explImg[i] && i==0) printf("Couldn't open '%s'\n",packGetFile("themes",tempStr) );

    if(graphics.explImg[i])
      graphics.brickExpl[i] = mkAni(graphics.explImg[i], 30,30, 0);
    else
      graphics.brickExpl[i] = mkAni(graphics.explImg[0], 30,30, 0);
  }

  //Per-Tile animations
  for(i=0; i < NUMTILES; i++)
  {
    sprintf(tempStr, "%s-tile%i.png", li->tileFile, i);
    graphics.aniImg[i] = loadImg( packGetFile("themes",tempStr) );

    graphics.tileAni[i] = mkAni(graphics.aniImg[i], 30,30, 80);

  }

  //Cursor
  graphics.curImg = loadImg( packGetFile( "themes/cursors",li->cursorName) );
  if( !graphics.curImg )
  {
      printf("Warning: Couldn't find cursor '%s'\n", packGetFile( "themes/cursors",li->cursorName));
      printf("Warning: Using deprecated PACKNAME/cursor.png instead\nPlease move to PACKNAME/themes/cursors/cursor.png");
      graphics.curImg = loadImg( packGetFile( ".","cursor.png") );
      if( !graphics.curImg )
      {
        printf("Error: Couldn't load cursor '%s'\n", packGetFile( ".","cursor.png"));
        cleanUpDraw();
        return(0);
      }

  }
  graphics.curSpr[0] = cutSprite(graphics.curImg, 0, 0, 28,28);
  graphics.curSpr[1] = cutSprite(graphics.curImg, 28, 0, 28,28);


  //Cut sprites
  for(i=0; i < NUMTILES; i++)
  {
    graphics.tiles[i] = cutSprite(graphics.tileImg, i*20, 0, 20,20);
  }

  //Load countdown
  graphics.countDownImg = loadImg( packGetFile(".","countdown.png") );
  if(!graphics.countDownImg)
  {
    printf("Error(5): couldn't load '%s'\n",packGetFile(".","countdown.png"));
    return(0);
  }
  //Cut countdown
  for(i=0; i < 4; i++)
  {
    graphics.countDownSpr[i] = cutSprite(graphics.countDownImg, 0,i*60, 140,60);
  }

  //Teleport path animation color gradient
  graphics.teleColorIndex=0;
  float f=255/TELEPATHNUMCOL;
  for(i=0; i < TELEPATHNUMCOL; i++)
  {
    graphics.teleColorTable[i] = SDL_MapRGB( screen->format, 0,(int)(255.0-(float)i*f),0  );
  }

  //The color white
  graphics.colWhite = SDL_MapRGB( screen->format, 255,255,255 );


  return(1);
}

void cleanUpDraw()
{
  int i;
  //Board image
  if(graphics.boardImg) SDL_FreeSurface(graphics.boardImg);
  graphics.boardImg=0;

  //Tile image
  if(graphics.tileImg) SDL_FreeSurface(graphics.tileImg);
  graphics.tileImg=0;

  //Tile sprites
  for(i=0; i < NUMTILES; i++)
  {
    if(graphics.tiles[i]) free(graphics.tiles[i]);
    graphics.tiles[i]=0;
  }

  //Cursor image
  if(graphics.curImg) SDL_FreeSurface(graphics.curImg);
  graphics.curImg=0;
  //Cursor sprite
  if(graphics.curSpr[0]) free(graphics.curSpr[0]);
  graphics.curSpr[0]=0;

  if(graphics.curSpr[1]) free(graphics.curSpr[1]);
  graphics.curSpr[1]=0;

  //Wall image
  if(graphics.wallImg) SDL_FreeSurface(graphics.wallImg);
  graphics.wallImg=0;

  //Wall sprites
  for(i=0; i < 16; i++)
  {
    if(graphics.walls[i]) free(graphics.walls[i]);
    graphics.walls[i]=0;
  }

  //Explosion
  for(i=0; i < BRICKSEND; i++)
  {
    //image
    if( graphics.explImg[i] ) SDL_FreeSurface( graphics.explImg[i] );
    graphics.explImg[i]=0;

    freeAni( graphics.brickExpl[i]);
  }

  //Countdown
  if(graphics.countDownImg) SDL_FreeSurface(graphics.countDownImg);
  graphics.countDownImg=0;

  for(i=0; i < 4; i++)
  {
    if(graphics.countDownSpr[i]) free(graphics.countDownSpr[i]);
    graphics.countDownSpr[i]=0;
  }
}

/* Permanently disables cursor, used for generating preview images */
#ifndef GP2X
void drawDisableCursor()
{
  if(graphics.curSpr[0]) free(graphics.curSpr[0]);
  if(graphics.curSpr[1]) free(graphics.curSpr[1]);
  graphics.curSpr[0] = 0;
  graphics.curSpr[1] = 0;
}
#endif


void draw(cursorType* cur, playField* pf, SDL_Surface* screen)
{
  int x,y;
  listItem* t; //general purpose, reusable
  psysSet_t ps;

  SDL_BlitSurface(graphics.boardImg , NULL, screen, &(setting()->bgPos) );

  for(x=0;x<NUMTILES;x++)
  {
    playAni(graphics.tileAni[x]);
  }

  //Draw static bricks
  for(y=0; y < FIELDSIZE; y++)
  {
    for(x=0; x < FIELDSIZE; x++)
    {
      //Bricks-Walls
      if(pf->board[x][y] && pf->board[x][y]->type != RESERVED)
      {
        //printf("X:%i Y:%i Type: %i\n",x,y,pf->board[x][y]->type);
        //Is it a wall?
        if(pf->board[x][y]->type == STDWALL)
        {
          //printf("I want to draw %i for(%i,%i) it has sprite: %i\n",pf->board[x][y]->wall,x,y,graphics.walls[pf->board[x][y]->wall]);
          drawSprite(screen, graphics.walls[pf->board[x][y]->wall], pf->board[x][y]->pxx, pf->board[x][y]->pxy);
        } else if(graphics.tiles[pf->board[x][y]->type-1])
        {
          if(graphics.tileAni[pf->board[x][y]->type-1])
          {
            drawAni(screen, graphics.tileAni[pf->board[x][y]->type-1], pf->board[x][y]->pxx-5, pf->board[x][y]->pxy-5);
          } else {
            drawSprite(screen, graphics.tiles[pf->board[x][y]->type-1], pf->board[x][y]->pxx, pf->board[x][y]->pxy);
          }
        }
      } /*else if( pf->board[x][y] && pf->board[x][y]->type == RESERVED )
      {
        drawSprite(screen, graphics.tiles[RESERVED-1], x*brickSize+boardOffsetX, y*brickSize+boardOffsetY);
      }*/
    }
  }

  //Draw moving bricks
  t=pf->movingList;
  brickType* b;
  while( (t = t->next) )
  {
    b=(brickType*)t->data;

    if(graphics.tileAni[b->type-1])
    {
      drawAni(screen, graphics.tileAni[b->type-1], b->pxx-5, b->pxy-5);
    } else {
      drawSprite(screen, graphics.tiles[b->type-1], b->pxx, b->pxy);
    }
  }

  //Particle systems that are between bricks and die animantion
  runParticlesLayer(screen, PSYS_LAYER_UNDERDEATHANIM);


  //Draw dying bricks, animation?
  t=pf->removeList;
  while( (t = t->next) )
  {
    b=(brickType*)t->data;
    //Draw base brick if time enough left
    if(b->tl > (pf->levelInfo->brick_die_ticks/2))
    {
      if(graphics.tileAni[b->type-1])
      {
        drawAni(screen, graphics.tileAni[b->type-1], b->pxx-5, b->pxy-5);
      } else {
        drawSprite(screen, graphics.tiles[b->type-1], b->pxx, b->pxy);
      }
    }


    int explFrame = 16*(pf->levelInfo->brick_die_ticks-b->tl)/pf->levelInfo->brick_die_ticks;
    drawAniFrame(screen, graphics.brickExpl[b->type-1], b->pxx-5, b->pxy-5,explFrame);

    //Spawn particles for brick death
    if(explFrame==8 && pf->levelInfo->brickDieParticles)
    {
      ps.layer=PSYS_LAYER_UNDERDEATHANIM;
      ps.x=b->pxx;
      ps.y=b->pxy;
      ps.vel=50;
      ps.life=750;
      ps.lifeVar=1000;
      ps.gravity=0;
      ps.bounce=0;
      ps.srcImg=graphics.tiles[b->type-1]->img;
      ps.srcRect=graphics.tiles[b->type-1]->clip;
      spawnParticleSystem(&ps);
    }

  }

  //Teleport overlay
  t = pf->levelInfo->teleList;
  telePort_t* tp;
  while( (t=t->next) )
  {
    tp = (telePort_t*)t->data;

    if(graphics.tileAni[TELESRC-1])
    {
      drawAni(screen, graphics.tileAni[TELESRC-1], boardOffsetX+20*tp->sx-5, boardOffsetY+20*tp->sy-5);
    } else {
      drawSprite(screen, graphics.tiles[TELESRC-1], boardOffsetX+20*tp->sx, boardOffsetY+20*tp->sy);
    }


    //if cursor is on it, draw the path too
    if(cur->x == tp->sx && cur->y == tp->sy)
    {
      drawTelePath( screen, tp, 1 );
    }
  }

  //Particles
  runParticles(screen);

  //Cursor
  updateCursor(cur);
  if(!cur->lock)
    drawSprite(screen, graphics.curSpr[0], cur->px, cur->py);
  else
    drawSprite(screen, graphics.curSpr[1], cur->px, cur->py);

  //Cursor "spot"
  if( getInpPointerState()->vpX > -1 )
  {
    plotPixel(screen, getInpPointerState()->vpX, getInpPointerState()->vpY-2, graphics.colWhite );
    plotPixel(screen, getInpPointerState()->vpX, getInpPointerState()->vpY+2, graphics.colWhite );

    plotPixel(screen, getInpPointerState()->vpX-2, getInpPointerState()->vpY, graphics.colWhite );
    plotPixel(screen, getInpPointerState()->vpX+2, getInpPointerState()->vpY, graphics.colWhite );
  }

  if(graphics.curSpr[0] && cur->moving )
  {
    ps.layer=PSYS_LAYER_TOP;
    ps.x=cur->px;
    ps.y=cur->py;
    ps.vel=50;
    ps.life=100;
    ps.lifeVar=100;
    ps.gravity=0;
    ps.srcImg=graphics.curSpr[0]->img;
    ps.srcRect=graphics.curSpr[0]->clip;

    spawnParticleSystem(&ps);
  }

}

static int lastShown=0;
void drawShowCountDown(SDL_Surface* screen,int i)
{
  psysSet_t ps;

  drawSprite(screen, graphics.countDownSpr[i], HSCREENW-140/2,HSCREENH-60/2);

  if(i!=lastShown)
  {
    ps.layer=PSYS_LAYER_TOP;
    ps.x=HSCREENW-140/2;
    ps.y=HSCREENH-60/2;
    ps.vel=20; // +/- in each dir
    ps.life=1000;
    ps.lifeVar=1300;
    ps.fade=0;
    ps.gravity=0;
    ps.bounce=1;      //If 1, particles will bounce off screen borders ( (vel * -1)/2 )
    ps.fadeColor=0x00;
    ps.srcImg=graphics.countDownSpr[i]->img;
    ps.srcRect=graphics.countDownSpr[i]->clip;

    spawnParticleSystem(&ps);

    lastShown=i;
  }

}

void drawBrick(SDL_Surface* screen, int brickNum, int x, int y)
{
  drawSprite(screen, graphics.tiles[brickNum-1], x,y);
}

struct boardGraphics_t* stealGfxPtr()
{
  return(&graphics);
}

void drawPath( SDL_Surface* screen, int sx, int sy, int dx, int dy, int animate )
{
    int x,y,/*dx,dy,*/cidx;
    int num=0;

    x=boardOffsetX + sx*20;
    y=boardOffsetY + sy*20;
    dx=boardOffsetX + dx*20;
    dy=boardOffsetY + dy*20;

    if(animate)
    {
      graphics.teleColorIndex+=2;
      if(graphics.teleColorIndex > TELEPATHNUMCOL)
        graphics.teleColorIndex = 0;
    }
    cidx=graphics.teleColorIndex;



    while(x!=dx || y!=dy)
    {
      if(x<dx) x++;
      if(x>dx) x--;

      if(y<dy) y++;
      if(y>dy) y--;

      if(animate)
      {
        cidx--;
        if(cidx<0)
          cidx=TELEPATHNUMCOL-1;
        plotPixel(screen, x+10,y+10, graphics.teleColorTable[cidx]);
      } else if(num%3==0) {
        plotPixel(screen, x+10,y+10, graphics.teleColorTable[0]);
      }
      num++;
    }

}

void drawTelePath(SDL_Surface* screen, telePort_t* tp, int animate)
{
    drawPath( screen, tp->sx, tp->sy,tp->dx, tp->dy, animate );
}

void drawAllTelePaths( SDL_Surface* screen, listItem* l )
{
  listItem* it = l;
  telePort_t* t;

  while( (it=it->next) )
  {
    t = (telePort_t*)it->data;
    drawTelePath(screen, t, 0);

  }
}
