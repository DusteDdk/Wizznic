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

#include "stars.h"
#include "pixel.h"
#include "sound.h"
#include "ticks.h"
#include "defs.h"

static list_t* stars;
static list_t* rockets;
//Setup 1000 stars
#define NUMSTARS 500
void initStars(SDL_Surface* screen)
{
  rockets = listInit(free);
  stars = listInit(free);
  star_t* star;
  int i;
  uint8_t col;
  for(i=0; i < NUMSTARS; i++)
  {
    star = malloc(sizeof(star_t));
  //  star->x = rand()%(SCREENW*10);
   // star->y = rand()%(SCREENH*10);

    star->x = rand()%(3200) + (HSCREENW-160)*10;
    star->y = rand()%(2400) + (HSCREENH-120)*10;


    star->sy = 0;
    col = rand()%229+26; //from 26 to 255
    star->sx = (int)( (float)(10.0/255.0)*(float)col );
    star->color = SDL_MapRGB(screen->format, col,col,col);
    listAppendData(stars, (void*)star); //Add star to list
  }
}

void starField(SDL_Surface* screen, int move)
{
  SDL_Rect dst;
  dst.x = HSCREENW-160;
  dst.y = HSCREENH-120;
  dst.w = 320;
  dst.h = 240;
  SDL_FillRect(screen, &dst, 0x00);
  star_t* star;
  listItem* it= &stars->begin;
  while( LISTFWD(stars,it) )
  {
    star=(star_t*)it->data;
    //Move star
    if(move)
    {
      star->x -= star->sx;
      //Out of screen?
      if(star->x < HSCREENW*10-1600)
      {
        //Give new y and reset x
        star->x = 3200+(HSCREENW-160)*10;
        star->y = rand()%(2400) + (HSCREENH-120)*10;
      }
    }
    //Draw
    plotPixel(screen, star->x/10, star->y/10, star->color);
  }
}


/* Fireworks effect */

static int nextExpl = 0; //Countdown
void fireWorks(SDL_Surface* screen)
{
  rocket_t* tempRocket;
  star_t* tempStar;
  listItem* it;

  uint32_t colWhite = SDL_MapRGB(screen->format, 255,255,255);
  uint32_t colYellow = SDL_MapRGB(screen->format, 255,255,0);

  /*
      New Rockets
                        */
  nextExpl -= getTicks();
  if(nextExpl < 1)
  {
    //Set new timer
    nextExpl = rand()%2000;
    //Fire a new rocket
    tempRocket = malloc(sizeof(rocket_t));
    //Set initial position at y 240, and some random x
    tempRocket->y=((HSCREENH+120)*10);
    tempRocket->x=rand()%(3200) + (HSCREENW-160)*10;
    //Set a direction that flies towards the middle
    tempRocket->sx = rand()%5;

    if(tempRocket->x > (HSCREENW*10) )
    {
      tempRocket->sx *= -1;
    }

    tempRocket->sy = 0-rand()%30-20;
    //Set life
    tempRocket->life=rand()%1000+250+10;

    tempRocket->p = listInit(free);
    //Init particles for explosion
    int i, r=rand()%100;
    for(i=0; i < r; i++)
    {
      tempStar = malloc( sizeof(star_t) );
      //Set dir to something random
      tempStar->sx = rand()%500-250;
      tempStar->sy = rand()%500-250;
      tempStar->color = SDL_MapRGB( screen->format, rand()%128+128,rand()%256,rand()%128);
      tempStar->life = rand()%3000+500;
      //Add to list
      listAppendData( tempRocket->p, (void*)tempStar );
    }
    //Add rocket to list
    listAppendData(rockets, (void*)tempRocket);

    //Play  launch sound
    sndPlay(SND_ROCKETLAUNCH, tempRocket->x/10);
  }

  /*
      Going through rockets and their particles
                                                  */
  it=&rockets->begin;
  listItem* itt;
  while( LISTFWD(rockets,it) )
  {
    tempRocket=(rocket_t*)it->data;
    //If rocket is still alive, fly it
    if(tempRocket->life > 0)
    {
      //Age
      tempRocket->life -= getTicks();
      //Set position for particles if it got too old
      if(tempRocket->life < 1)
      {
        itt=&(tempRocket->p->begin);
        while( LISTFWD(tempRocket->p, itt) )
        {
          tempStar=(star_t*)itt->data;
          tempStar->x = tempRocket->x*10;
          tempStar->y = tempRocket->y*10;
        }
        //Play "Explosion" sound
        sndPlay(SND_ROCKETBOOM, tempRocket->x/10);
      }
      //Fly
      tempRocket->x += tempRocket->sx;
      tempRocket->y += tempRocket->sy;
      //Draw
      plotPixel(screen, tempRocket->x/10, tempRocket->y/10, colWhite );
      plotPixel(screen, tempRocket->x/10, tempRocket->y/10+1, colYellow );

    } else {
      //iterate through stars
      itt=&(tempRocket->p->begin);
      int liveStars=0;
      while( LISTFWD(tempRocket->p, itt) )
      {
        tempStar=(star_t*)itt->data;
        //alive?
        if(tempStar->life > 0)
        {
          //Fly

          tempStar->x += tempStar->sx/10;
          tempStar->y += tempStar->sy/10;

          //Gravity
          if(tempStar->y < 10)
              tempStar->y += 20;

          //Draw
          if(tempStar->life > 1000 || tempStar->life % 2 == 0)
            plotPixel(screen, tempStar->x/100, tempStar->y/100, tempStar->color);
          else if(tempStar->life % 3 == 0)
            plotPixel(screen, tempStar->x/100, tempStar->y/100, colWhite);

          //age
          tempStar->life -= getTicks();

          liveStars++;
        } //alive
      }
      //Check if it should still survice
      if(liveStars == 0)
      {
        //Remove stars
        listFree(tempRocket->p);

        //Remove rocket
        it=listRemoveItem(rockets, it, LIST_PREV);
      }
    } //Sim rocket stars
  } //iterate through rockets

}
