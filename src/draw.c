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

#include "draw.h"
#include "pack.h"
#include "defs.h"
#include "settings.h"
#include "input.h"

struct boardGraphics_t graphics;

int initDraw(levelInfo_t* li, SDL_Surface* screen)
{
  char tempStr[512];
  int i;

  //Background image
  graphics.boardImg = loadImg( packGetFile("themes",li->bgFile) );
  if(!graphics.boardImg)
  {
    printf("Couldn't load board file:'%s'\n", packGetFile("themes",li->bgFile) );
    cleanUpDraw();
    return(0);
  }


  //The image into which we blit background and bricks
  graphics.background = SDL_ConvertSurface( graphics.boardImg, graphics.boardImg->format, graphics.boardImg->flags );
  if( !graphics.background )
  {
    printf("ERROR: Could not allocate SDL_Surface for background.");
    cleanUpDraw();
    return(0);
  }
  //Some surfaces has flags set that prevent propper blitting
  graphics.background->flags=0x00;

  //Tileset
  sprintf(tempStr, "%s.png", li->tileBase);
  graphics.tileImg = loadImg( packGetFile("themes",tempStr) );
  if(!graphics.tileImg)
  {
    printf("Couldn't load tile file:'%s'\n", packGetFile("themes",tempStr) );
    cleanUpDraw();
    return(0);
  }

  //Cut tiles into sprites
  for(i=0; i < NUMTILES; i++)
  {
    graphics.tiles[i] = cutSprite(graphics.tileImg, i*20, 0, 20,20);
  }

  sprintf(tempStr, "%s.png", li->wallBase);
  graphics.wallsImg = loadImg( packGetFile("themes",tempStr) );
  if(graphics.wallsImg)
  {
    for(i=0; i < 13; i++)
    {
      graphics.walls[i] =  cutSprite(graphics.wallsImg, i*20,0, 20, 20);
    }
  } else {
    printf("Error: No edges for: %s (File not found: %s)\n",li->wallBase,tempStr );
    cleanUpDraw();
    return(0);
  }


  //Explosions, reuse R as frame number index
  for(i=0; i < BRICKSEND; i++)
  {
    //Open explosion
    sprintf(tempStr, "%s%02i.png", li->explBase, i);
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
    sprintf(tempStr, "%s-tile%02i.png", li->tileBase, i);
    graphics.aniImg[i] = loadImg( packGetFile("themes",tempStr) );

    graphics.tileAni[i] = mkAni(graphics.aniImg[i], 30,30, 80);

  }

  //Cursor
  graphics.curImg = loadImg( packGetFile( "themes/cursors",li->cursorFile) );
  if( !graphics.curImg )
  {
      printf("Warning: Couldn't find cursor '%s'\n", packGetFile( "themes/cursors",li->cursorFile));
  }
  graphics.curSpr[0] = cutSprite(graphics.curImg, 0, 0, 28,28);
  graphics.curSpr[1] = cutSprite(graphics.curImg, 28, 0, 28,28);


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

  return(1);
}

void cleanUpDraw()
{
  int i;
  //Board image
  if(graphics.boardImg) SDL_FreeSurface(graphics.boardImg);
  graphics.boardImg=0;

  if(graphics.background) SDL_FreeSurface(graphics.background);
  graphics.background=0;

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

  //Walls
  if(graphics.wallsImg) SDL_FreeSurface(graphics.wallsImg);
    graphics.wallsImg=0;

  //Wall sprites
  for(i=0; i < 13; i++)
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
#if !defined (GP2X) && !defined (WIZ) && !defined (PANDORA)
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

  //Check if we should draw walls
  if( pf->newWalls )
  {
    pf->newWalls=0;
    SDL_BlitSurface(graphics.boardImg , NULL, graphics.background, NULL );

    //Draw static bricks
    for(y=0; y < FIELDSIZE; y++)
    {
      for(x=0; x < FIELDSIZE; x++)
      {
        //Bricks-Walls
        if(pf->board[x][y] && pf->board[x][y]->type != RESERVED)
        {
          //We treat walls/glue/oneways/switches/evilbricks/copybricks and rembricks as walls (they will have the walltile defined)

          if( isWall(pf, x, y) )
          {
            int i;
            //Draw middle wall (idx 0)
            drawSprite(graphics.background, graphics.walls[0], pf->board[x][y]->pxx-(HSCREENW-160), pf->board[x][y]->pxy-(HSCREENH-120) );
            //Draw edges (if any)
            for(i=1; i < 13; i++)
            {
              if(pf->board[x][y]->edges & (1<<i) )
              {
                drawSprite(graphics.background, graphics.walls[i], pf->board[x][y]->pxx-(HSCREENW-160), pf->board[x][y]->pxy-(HSCREENH-120) );
              }
            }
          }
        }
      }
    }
  }


  //Blit background
  SDL_BlitSurface(graphics.background , NULL, screen, &(setting()->bgPos) );

  //Advance animations
  for(x=0;x<NUMTILES;x++)
  {
    playAni(graphics.tileAni[x]);
  }

  //Draw bricks that are not moving around
  for(y=0; y < FIELDSIZE; y++)
  {
    for(x=0; x < FIELDSIZE; x++)
    {

      if(pf->board[x][y] && pf->board[x][y]->type != RESERVED)
      {
        if( pf->board[x][y]->type != STDWALL && graphics.tiles[pf->board[x][y]->type-1])
        {
          //We draw the animated extra-tiles if they exist.
          if(graphics.tileAni[pf->board[x][y]->type-1])
          {
            if( !isSwitch( pf->board[x][y] ) )
            {
              drawAni(screen, graphics.tileAni[pf->board[x][y]->type-1], pf->board[x][y]->pxx-5, pf->board[x][y]->pxy-5);
            } else {
              //We only end here when it's a switch
              if( (pf->board[x][y]->type==SWON)?pf->board[x][y]->isActive:!pf->board[x][y]->isActive)
              {
                drawAni(screen, graphics.tileAni[SWON-1], pf->board[x][y]->pxx-5, pf->board[x][y]->pxy-5);
              } else {
                drawAni(screen, graphics.tileAni[SWOFF-1], pf->board[x][y]->pxx-5, pf->board[x][y]->pxy-5);
              }
            }
          //Fall back to the static non-moving tiles if no animation is found.
          } else {
            if( !isSwitch( pf->board[x][y] ) )
            {
              drawSprite(screen, graphics.tiles[pf->board[x][y]->type-1], pf->board[x][y]->pxx, pf->board[x][y]->pxy);
            } else {
              if( (pf->board[x][y]->type==SWON)?pf->board[x][y]->isActive:!pf->board[x][y]->isActive)
              {
                drawSprite(screen, graphics.tiles[SWON-1], pf->board[x][y]->pxx, pf->board[x][y]->pxy);
              } else {
                drawSprite(screen, graphics.tiles[SWOFF-1], pf->board[x][y]->pxx, pf->board[x][y]->pxy);
              }
            }
          }
        } // not a wall.
      } //Not a reserved brick.
      /*else if( pf->board[x][y] && pf->board[x][y]->type == RESERVED )
      {
        drawSprite(screen, graphics.tiles[RESERVED-1], x*brickSize+boardOffsetX, y*brickSize+boardOffsetY);
      }*/

      //if cursor is on it, draw the path too
      if( cur->x == x && cur->y == y && isSwitch(pf->board[x][y]) && pf->levelInfo->showSwitchPath )
      {
        listItem* it = &(pf->levelInfo->switchList->begin);
        while( LISTFWD(pf->levelInfo->switchList, it) )
        {
          telePort_t* tp = (telePort_t*)it->data;
          if(tp->sx==x && tp->sy==y)
          {
            drawTelePath( screen, tp, 1 );
            break;
          }
        }

      }

    }
  } //xy loop

  //Draw moving bricks
  t=&pf->movingList->begin;
  brickType* b;
  while( LISTFWD(pf->movingList,t) )
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
  t=&pf->removeList->begin;

  while( LISTFWD(pf->removeList,t) )
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
      ps.layer=pf->levelInfo->brickDieParticles;
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
  t = &pf->levelInfo->teleList->begin;
  telePort_t* tp;
  while( LISTFWD(pf->levelInfo->teleList,t) )
  {
    tp = (telePort_t*)t->data;

    if(graphics.tileAni[TELESRC-1])
    {
      drawAni(screen, graphics.tileAni[TELESRC-1], boardOffsetX+20*tp->sx-5, boardOffsetY+20*tp->sy-5);
    } else {
      drawSprite(screen, graphics.tiles[TELESRC-1], boardOffsetX+20*tp->sx, boardOffsetY+20*tp->sy);
    }


    //if cursor is on it, draw the path too
    if( cur->x == tp->sx && cur->y == tp->sy && pf->levelInfo->showTelePath )
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

void drawAllTelePaths( SDL_Surface* screen, list_t* l )
{
  listItem* it = &l->begin;
  telePort_t* t;

  while( LISTFWD(l,it) )
  {
    t = (telePort_t*)it->data;
    drawTelePath(screen, t, 0);
  }
}
