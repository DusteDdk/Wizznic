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

#include "skipleveldialog.h"
#include "input.h"
#include "sprite.h"
#include "input.h"
#include "defs.h"
#include "pack.h"
#include "player.h"
#include "settings.h"

static SDL_Surface* skipDialogSurface=NULL;

static SDL_Surface* restartHighlightSurface=NULL;
static SDL_Surface* skipHighlightSurface=NULL;
static aniType* restartHighlightAni=NULL;
static aniType* skipHighlightAni=NULL;

static int lastPackNum=1;
static int selected=0;
static int btnClicked=0;

static SDL_Rect restartBoxBtn;
static SDL_Rect skipBoxBtn;

void loadSkipLevelDialog()
{
  //Do we need to load the dialog?
  if( lastPackNum == -1 || lastPackNum != packState()->selected )
  {
    lastPackNum = packState()->selected;
    //Free existing resources if they are loaded
    if( skipDialogSurface ) SDL_FreeSurface(skipDialogSurface);
    if( restartHighlightSurface ) SDL_FreeSurface(restartHighlightSurface);
    if( skipHighlightSurface ) SDL_FreeSurface(skipHighlightSurface);
    if( restartHighlightAni ) freeAni(restartHighlightAni);
    if( skipHighlightAni ) freeAni(skipHighlightAni);

    //Load new images
    skipDialogSurface = loadImg( packGetFile(".","skipleveldialog.png") );
    restartHighlightSurface = loadImg( packGetFile(".", "skipleveldialog-hl0.png") );
    skipHighlightSurface = loadImg( packGetFile(".", "skipleveldialog-hl1.png") );

    //Make animations
    restartHighlightAni = mkAni(restartHighlightSurface, 48,44,150);
    skipHighlightAni = mkAni(skipHighlightSurface, 48,44,150);

    //Define positions used for checking pointer against the buttons
    restartBoxBtn.x = HSCREENW-45;
    restartBoxBtn.y = HSCREENH-21;
    restartBoxBtn.w = HSCREENW;
    restartBoxBtn.h = HSCREENH+20;

    skipBoxBtn.x = HSCREENW;
    skipBoxBtn.y = HSCREENH-21;
    skipBoxBtn.w = HSCREENW+46;
    skipBoxBtn.h = HSCREENH+20;

    btnClicked=0;

  }
}

int skipLevelDialog(SDL_Surface* screen)
{
  loadSkipLevelDialog();

  if( getButton(C_LEFT) || getButton(C_RIGHT) )
  {
    resetBtn(C_LEFT);
    resetBtn(C_RIGHT);
    if( selected==0 )
    {
      selected=1;
    } else {
      selected=0;
    }
  }

  if(getInpPointerState()->timeSinceMoved<POINTER_SHOW_TIMEOUT)
  {
    if( isPointerInBox(&skipBoxBtn))
    {
      selected=1;
      if(isBoxClicked( &skipBoxBtn))
      {
        btnClicked=1;
      }
    } else if( isPointerInBox( &restartBoxBtn))
    {
      selected=0;
      if(isBoxClicked( &restartBoxBtn))
      {
        btnClicked=1;
      }
    }
  }

  if( getButton(C_BTNB) || btnClicked )
  {
    resetBtn(C_BTNB);
    resetMouseBtn();
    btnClicked = 0;
    player()->timeouts=0;
    return(selected);
  }

  SDL_BlitSurface( skipDialogSurface, 0, screen, &(setting()->bgPos) );

  if(selected==0)
  {
    playAni(restartHighlightAni);
    drawAni(screen, restartHighlightAni, HSCREENW-48,HSCREENH-22);
  } else {
    playAni(skipHighlightAni);
    drawAni(screen, skipHighlightAni, HSCREENW,HSCREENH-22);
  }

  //Caller only reacts to 0 or 1. We use 2 to indicate that no selection has been made yet.
  return(2);
}
