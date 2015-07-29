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

#include "time.h"
#include "text.h"

#include "defs.h"

static int lastTick=0; //SDL clock
static int ticks=0; //ticks since last frame

static int frames=0;
void frameStart()
{
  ticks = SDL_GetTicks() - lastTick;
  lastTick = SDL_GetTicks();
  frames++;
}

static int fpsSecondCounter=0;
static int fps=0;
static char fpsStr[16] = { '0','0','\0' };

int getTicks()
{
  return(ticks);
}

int getTimeSinceFrameStart()
{
  return( SDL_GetTicks() - lastTick );
}


void drawFPS(SDL_Surface* scr)
{
  fps++;
  fpsSecondCounter+=getTicks();
  if(fpsSecondCounter > 999)
  {
    sprintf(fpsStr, "%i Fps", fps);
    fps=0;
    fpsSecondCounter=0;
  }

  txtWrite(scr,FONTSMALL, fpsStr, HSCREENW-160,HSCREENH-120);
}
