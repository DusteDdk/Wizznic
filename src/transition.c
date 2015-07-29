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

#include "transition.h"
#include "pixel.h"
#include "ticks.h"
#include "list/list.h"
#include "particles.h"

struct transition_s {
   SDL_Surface* sur;
   int_fast8_t type;
   int time;
   int timeLeft;
   int_fast8_t reverse;
};

typedef struct transition_s transition_t;

transition_t t;

void initTransition()
{
  t.sur=NULL;
  t.timeLeft=0;
  t.time=0;
  t.sur=NULL;
}

#define _TRANSITION_TYPE_ROLL 253

void startTransition(SDL_Surface* scr, uint_fast8_t type, uint_fast16_t time)
{
  struct psysSet_s psys;

  t.type=(type==TRANSITION_TYPE_RANDOM)?rand()%NUM_TRANSITIONS:type;
  t.time=time;
  t.timeLeft=t.time;
  t.reverse=0;

  if( t.sur != NULL )
  {
    SDL_FreeSurface(t.sur);
  }

  switch( t.type )
  {
    case TRANSITION_TYPE_DISSOLVE:
      clearParticles();
      psys.bounce=0;
      psys.fade=0;
      psys.gravity=0;
      psys.layer=PSYS_LAYER_TOP;
      psys.life=time;
      psys.lifeVar=time;
      psys.srcImg=scr;
      psys.x=(HSCREENW-160);
      psys.y=(HSCREENH-120);
      psys.srcRect.x=(HSCREENW-160);
      psys.srcRect.y=(HSCREENH-120);
      psys.srcRect.w=320;
      psys.srcRect.h=240;
      psys.vel=0;

      spawnParticleSystem(&psys);
    break;
    case TRANSITION_TYPE_ROLL_IN:
    case TRANSITION_TYPE_CURTAIN_DOWN:
     t.reverse=1;
     /* no break */
    case TRANSITION_TYPE_ROLL_OUT:
    case TRANSITION_TYPE_CURTAIN_UP:
      t.sur = SDL_ConvertSurface( scr, scr->format, scr->flags );
      //Some surfaces has flags set that prevent propper blitting
      t.sur->flags=0x00;
    break;
  }
}

void runTransition(SDL_Surface* scr)
{
  int x;
  if( t.timeLeft == 0 )
    return;

  t.timeLeft -= getTicks();
  if( t.timeLeft < 1 )
    t.timeLeft=0;

  SDL_Rect r,rr;
  SDL_Surface* tmpSurf;

  switch(t.type)
  {
    case TRANSITION_TYPE_CURTAIN_UP:
    case TRANSITION_TYPE_CURTAIN_DOWN:
      rr.x=(HSCREENW-160);
      x=((float)(t.timeLeft)/(float)(t.time))*(float)(240);

      r.x=(HSCREENW-160);
      r.w=(320);


      if( !t.reverse )
      {
        r.h=x;
        r.y=(HSCREENH-120);
        rr.y=(HSCREENH-120);
      } else {
        r.h=HSCREENH+120;
        r.y=HSCREENH+120-x;
        rr.y=HSCREENH+120-x;
      }
      SDL_BlitSurface(t.sur, &r, scr, &rr );
    break;
    case TRANSITION_TYPE_DISSOLVE:
      runParticles(scr);
    break;
    case TRANSITION_TYPE_ROLL_IN:
      x=((float)(t.timeLeft)/(float)(t.time))*(float)(320);

      tmpSurf = SDL_ConvertSurface( scr, scr->format, scr->flags );
      tmpSurf->flags=0x00;
      r.y=(HSCREENH-120);
      rr.y=(HSCREENH-120);
      r.h=240;
      rr.h=240;

      rr.x=HSCREENW+160-x;
      r.x = (HSCREENW-160);
      r.w = x;
      SDL_BlitSurface(t.sur, &r, scr, &rr );

      rr.x=HSCREENW-160;
      r.x = HSCREENW-160+x;
      r.w = 320-x;
      SDL_BlitSurface(tmpSurf,&r, scr, &rr );

      SDL_FreeSurface(tmpSurf);
    break;

    case TRANSITION_TYPE_ROLL_OUT:
      x=((float)(t.timeLeft)/(float)(t.time))*(float)(320);

      tmpSurf = SDL_ConvertSurface( scr, scr->format, scr->flags );
      tmpSurf->flags=0x00;
      r.y=(HSCREENH-120);
      rr.y=(HSCREENH-120);
      r.h=240;
      rr.h=240;

      rr.x=HSCREENW-160;
      r.x = (HSCREENW+160)-x;
      r.w = x;
      SDL_BlitSurface(t.sur, &r, scr, &rr );

      rr.x=HSCREENW-160+x;
      r.x = HSCREENW-160;
      r.w = 320-x;
      SDL_BlitSurface(tmpSurf,&r, scr, &rr );

      SDL_FreeSurface(tmpSurf);
    break;

  }
}

int_fast8_t  transitionActive()
{
  return(t.timeLeft<1?0:1);
}
