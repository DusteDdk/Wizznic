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

#include "input.h"
#include "ticks.h"

#include "defs.h"

#include "settings.h"

static inpPointerState_t inpPointer;
inline inpPointerState_t* getInpPointerState()
{
  return(&inpPointer);
}

inline void resetMouseBtn()
{
  inpPointer.isDown=0;
}


static struct {
  int button;
  int state;
  int time;
} button[C_NUM];

inline int getButton(int btn)
{
  return(button[btn].state);
}

inline void setButton(int btn)
{
  button[btn].state=1;
  button[btn].time=0;
}


inline void resetBtnTimer(int btn)
{
  button[btn].time=0;
}

inline void resetBtn(int btn)
{
  button[btn].time=0;
  button[btn].state=0;
}

void resetBtnAll()
{
  int i;
  for(i=0; i < C_NUM; i++)
  {
    resetBtn(i);
  }
}

inline int getBtnTime(int btn)
{
  return(button[btn].time);
}

int runControls()
{
  SDL_Event event;
  int i;

  //Loop through buttons to update hold-down time
  for(i=0; i < C_NUM; i++)
  {
    if( button[i].state )
    {
      button[i].time += getTicks();
    }
  }

  //Update pointer-holddown time
  if(inpPointer.isDown)
  {
    inpPointer.downTime += getTicks();
  }

  if( inpPointer.timeSinceMoved < POINTER_SHOW_TIMEOUT)
  inpPointer.timeSinceMoved +=getTicks();

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
        #if defined (GP2X) || defined (WIZ)
        case SDL_JOYBUTTONDOWN:
          for(i=0; i < C_NUM; i++)
          {
            if( event.jbutton.button == button[i].button )
            {
              button[i].state=1;
              button[i].time=0;
            }
          }

          switch( event.jbutton.button )
          {
           case GP2X_BUTTON_UPLEFT:
              button[C_UP].state=1;
              button[C_UP].time=0;
              button[C_LEFT].state=1;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_UPRIGHT:
              button[C_UP].state=1;
              button[C_UP].time=0;
              button[C_RIGHT].state=1;
              button[C_RIGHT].time=0;
            break;
            case GP2X_BUTTON_DOWNLEFT:
              button[C_DOWN].state=1;
              button[C_DOWN].time=0;
              button[C_LEFT].state=1;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_DOWNRIGHT:
              button[C_DOWN].state=1;
              button[C_DOWN].time=0;
              button[C_RIGHT].state=1;
              button[C_RIGHT].time=0;
            break;
            case GP2X_BUTTON_VOLDOWN:
              //WIZ_AdjustVolume( VOLUME_DOWN );
              applySettings();
            break;
            case GP2X_BUTTON_VOLUP:
              //WIZ_AdjustVolume( VOLUME_UP );
            applySettings();
            break;
          }
        break;

        case SDL_JOYBUTTONUP:
          for(i=0; i < C_NUM; i++)
          {
            if( event.jbutton.button == button[i].button )
            {
              button[i].state=0;
              button[i].time=0;
            }
          }
          switch( event.jbutton.button )
          {
            case GP2X_BUTTON_UPLEFT:
              button[C_UP].state=0;
              button[C_UP].time=0;
              button[C_LEFT].state=0;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_UPRIGHT:
              button[C_UP].state=0;
              button[C_UP].time=0;
              button[C_RIGHT].state=0;
              button[C_RIGHT].time=0;
            break;
            case GP2X_BUTTON_DOWNLEFT:
              button[C_DOWN].state=0;
              button[C_DOWN].time=0;
              button[C_LEFT].state=0;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_DOWNRIGHT:
              button[C_DOWN].state=0;
              button[C_DOWN].time=0;
              button[C_RIGHT].state=0;
              button[C_RIGHT].time=0;
            break;
          }
        break;

        //Keyboard
        #else
        case SDL_KEYDOWN:
          for(i=0; i < C_NUM; i++)
          {
            if( event.key.keysym.sym == button[i].button )
            {
              button[i].state=1;
              button[i].time=0;
            }
          }
        break;
        case SDL_KEYUP:
          for(i=0; i < C_NUM; i++)
          {
            if( event.key.keysym.sym == button[i].button )
            {
              button[i].state=0;
              button[i].time=0;
            }
          }
        break;
        #endif

        //Handle pointer events
        case SDL_MOUSEBUTTONDOWN:
          if( inpPointer.curX > -1 && inpPointer.curX< 11 &&
              inpPointer.curY > -1 && inpPointer.curY < 11 )
          {
            inpPointer.downTime=0;
            inpPointer.isDown=1;
          }
        break;
        case SDL_MOUSEBUTTONUP:
          inpPointer.isDown=0;
        break;
        case SDL_MOUSEMOTION:

          inpPointer.vpX = (event.motion.x/setting()->scaleFactor);
          inpPointer.vpY = (event.motion.y/setting()->scaleFactor);

          inpPointer.curX = (inpPointer.vpX-(boardOffsetX*setting()->scaleFactor))/(20);
          inpPointer.curY = (inpPointer.vpY-(boardOffsetY*setting()->scaleFactor))/(20);

          inpPointer.timeSinceMoved=0;

          if( inpPointer.curX < 0 ) inpPointer.curX = 0;
          if( inpPointer.curX > 10 ) inpPointer.curX = 10;
          if( inpPointer.curY < 0 ) inpPointer.curY = 0;
          if( inpPointer.curY > 10 ) inpPointer.curY = 10;
        break;


        case SDL_QUIT:
          return(1);
        break;
    }
  }

  return(0);
}

void initControls()
{
  button[C_UP].button = PLATFORM_BUTTON_UP;
  button[C_DOWN].button = PLATFORM_BUTTON_DOWN;
  button[C_LEFT].button = PLATFORM_BUTTON_LEFT;
  button[C_RIGHT].button = PLATFORM_BUTTON_RIGHT;
  button[C_BTNX].button = PLATFORM_BUTTON_X;
  button[C_BTNY].button = PLATFORM_BUTTON_Y;
  button[C_BTNA].button = PLATFORM_BUTTON_A;
  button[C_BTNB].button = PLATFORM_BUTTON_B;
  button[C_SHOULDERA].button = PLATFORM_SHOULDER_LEFT;
  button[C_SHOULDERB].button = PLATFORM_SHOULLER_RIGHT;
  button[C_BTNMENU].button = PLATFORM_BUTTON_MENU;
  button[C_BTNSELECT].button = PLATFORM_BUTTON_SELECT;
  button[C_BTNVOLUP].button = PLATFORM_BUTTON_VOLUP;
  button[C_BTNVOLDOWN].button = PLATFORM_BUTTON_VOLDOWN;


  memset( &inpPointer, 0, sizeof(inpPointerState_t) );
  inpPointer.timeSinceMoved=POINTER_SHOW_TIMEOUT;
}

int_fast8_t isPointerClicked()
{
  if( inpPointer.isDown && inpPointer.downTime==0 )
  {
    return(1);
  }
  return(0);
}

int_fast8_t isBoxClicked( SDL_Rect* r )
{
  if( isPointerClicked() )
  {
    if( r->x < inpPointer.vpX && r->w > inpPointer.vpX && r->y < inpPointer.vpY && r->h > inpPointer.vpY )
    {
      return(1);
    }
  }
  return(0);
}
