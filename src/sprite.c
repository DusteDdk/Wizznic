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

#include "sprite.h"
#include "ticks.h"
#include "pack.h"

SDL_Surface* loadImg( const char* fileName )
{
  //Load the surface
  SDL_Surface* unoptimized = NULL;
  SDL_Surface* optimized = NULL;

  if( isFile(fileName) )
  {
#ifdef DEBUG
    printf("loadImg(); Open: %s\n",fileName);
#endif
    unoptimized = IMG_Load( fileName );

    if(unoptimized!=NULL)
    {
      //Create optimized.
      optimized = SDL_DisplayFormat( unoptimized );

      //Destroy old img
      SDL_FreeSurface( unoptimized );

      if(optimized!=NULL)
      {
        //Set colorkey for cheap transparency
        SDL_SetColorKey( optimized, SDL_SRCCOLORKEY, SDL_MapRGB( optimized->format, 0, 0xFF, 0xFF ) );
      }
    } else {
      printf("loadImg(%s) (IMG_Load) failed: %s\n", fileName, IMG_GetError());
    }
  }

  return(optimized);
}

spriteType* cutSprite(SDL_Surface* img, int x,int y, int w, int h)
{
  //Did we get a valid surface?
  if(!img) return(NULL);

  //Allocate memory for the sprite
  spriteType* tspr = malloc( sizeof(spriteType) );
  if(tspr != NULL)
  {
    tspr->img = img;
    tspr->clip.x = x;
    tspr->clip.y = y;
    tspr->clip.w = w;
    tspr->clip.h = h;
  } else {
    printf("Error, couldn't malloc %lu bytes.\n",(long unsigned int)sizeof(spriteType));
  }

  return(tspr);
}

void drawSprite(SDL_Surface* scr, spriteType* spr, int x, int y)
{
  if(!spr) return;
  SDL_Rect pos;
  pos.x = x;
  pos.y = y;
  SDL_BlitSurface( spr->img, &spr->clip, scr, &pos );
}

aniType* mkAni(SDL_Surface*img, int w,int h, int ticksPerFrame)
{
  if(!img)
  {
    return(NULL);
  }

  int numFrames = img->w/w;
  int i;

  aniType* ani = malloc( sizeof(aniType) );
  memset( ani, 0, sizeof(aniType) );


  ani->numFrames=numFrames;
  ani->numTicks=ticksPerFrame;
  ani->spr = malloc( sizeof(spriteType*) * numFrames );

  for(i=0;i<numFrames;i++)
  {
    ani->spr[i] = cutSprite(img, i*w,0, w,h);
  }

  return(ani);
}

void freeAni( aniType* ani )
{
  int i;
  if(!ani) return;
  for(i=0;i<ani->numFrames;i++)
  {
    free(ani->spr[i]);
  }
  free(ani);
}


void playAni(aniType* ani)
{
  if(!ani) return;
  //Progress to next frame?
  ani->tick += getTicks();
  if( ani->tick >= ani->numTicks )
  {
    ani->tick=0;
    ani->frame++;
    if(ani->frame==ani->numFrames)
    {
      ani->frame=0;
    }
  }
}
void drawAni(SDL_Surface* screen, aniType* ani, int x, int y)
{
  if(!ani) return;
  drawSprite(screen, ani->spr[ani->frame],x,y);
}

void drawAniFrame(SDL_Surface* screen, aniType* ani, int x, int y, int frame)
{
  if(!ani) return;
  drawSprite(screen, ani->spr[frame],x,y);
}
