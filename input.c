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

#if defined(PSP)
  #include "platform/pspspec.h"
#elif defined(GP2X)
  #include "platform/gp2x.h"
#elif defined(WIZ)
  #include "platform/wiz.h"
#endif

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
        #if defined (GP2X) || defined (PSP) || defined (WIZ)
        case SDL_JOYBUTTONDOWN:
          for(i=0; i < C_NUM; i++)
          {
            if( event.jbutton.button == button[i].button )
            {
              button[i].state=1;
              button[i].time=0;
            }
          }
          #if defined (GP2X) || defined (WIZ)
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
        #endif
        case SDL_JOYBUTTONUP:
          for(i=0; i < C_NUM; i++)
          {
            if( event.jbutton.button == button[i].button )
            {
              button[i].state=0;
              button[i].time=0;
            }
          }
          #if defined (GP2X) || defined (WIZ)
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
        #endif

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
          inpPointer.startX = ((event.button.x/setting()->scaleFactor)-boardOffsetX)/(20);
          inpPointer.startY = ((event.button.y/setting()->scaleFactor)-boardOffsetY)/(20);
          if( inpPointer.startX > -1 && inpPointer.startX < 11 &&
              inpPointer.startY > -1 && inpPointer.startY < 11 )
          {
            inpPointer.curX = inpPointer.startX;
            inpPointer.curY = inpPointer.startY;
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

          inpPointer.curX = (inpPointer.vpX-boardOffsetX)/(20);
          inpPointer.curY = (inpPointer.vpY-boardOffsetY)/(20);

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

  #if defined (GP2X) || defined (WIZ)
    button[C_UP].button = GP2X_BUTTON_UP;
    button[C_DOWN].button = GP2X_BUTTON_DOWN;
    button[C_LEFT].button =  GP2X_BUTTON_LEFT;
    button[C_RIGHT].button = GP2X_BUTTON_RIGHT;
    button[C_BTNY].button = GP2X_BUTTON_Y;
    button[C_BTNX].button = GP2X_BUTTON_X;
    button[C_BTNA].button = GP2X_BUTTON_A;
    button[C_BTNB].button = GP2X_BUTTON_B;
    button[C_SHOULDERA].button = GP2X_BUTTON_L;
    button[C_SHOULDERB].button = GP2X_BUTTON_R;
    button[C_BTNMENU].button = GP2X_BUTTON_START;
    button[C_BTNSELECT].button = GP2X_BUTTON_SELECT;
    button[C_BTNVOLUP].button = GP2X_BUTTON_VOLUP;
    button[C_BTNVOLDOWN].button = GP2X_BUTTON_VOLDOWN;
  #elif defined (PSP)
    button[C_UP].button = PSP_BUTTON_UP;
    button[C_DOWN].button = PSP_BUTTON_DOWN;
    button[C_LEFT].button =  PSP_BUTTON_LEFT;
    button[C_RIGHT].button = PSP_BUTTON_RIGHT;
    button[C_BTNY].button = PSP_BUTTON_TRIANGLE;
    button[C_BTNX].button = PSP_BUTTON_SQUARE;
    button[C_BTNA].button = PSP_BUTTON_CIRCLE;
    button[C_BTNB].button = PSP_BUTTON_CROSS;
    button[C_SHOULDERA].button = PSP_BUTTON_LTRIGGER;
    button[C_SHOULDERB].button = PSP_BUTTON_RTRIGGER;
    button[C_BTNMENU].button = PSP_BUTTON_START;
    button[C_BTNSELECT].button = PSP_BUTTON_SELECT;
    button[C_BTNVOLUP].button = GP2X_BUTTON_VOLUP;
    button[C_BTNVOLDOWN].button = GP2X_BUTTON_VOLDOWN;
  #elif defined (MAME_CTRL)
    #warning "Compiling with MAME keys"
    button[C_UP].button = SDLK_UP;
    button[C_DOWN].button = SDLK_DOWN;
    button[C_LEFT].button = SDLK_LEFT;
    button[C_RIGHT].button = SDLK_RIGHT;
    button[C_BTNX].button = SDLK_SPACE; //b4
    button[C_BTNY].button = SDLK_LSHIFT;//b3
    button[C_BTNA].button = SDLK_LALT;  //b2
    button[C_BTNB].button = SDLK_LCTRL; //b1
    button[C_SHOULDERA].button = SDLK_8;
    button[C_SHOULDERB].button = SDLK_9;
    button[C_BTNMENU].button = SDLK_1;  //p1start
    button[C_BTNSELECT].button = SDLK_2;//p2start
    button[C_BTNVOLUP].button = SDLK_a;
    button[C_BTNVOLDOWN].button = SDLK_s;
  // Added by farox for pandora port
  #elif defined (PANDORA)
    button[C_UP].button = SDLK_UP;
    button[C_DOWN].button = SDLK_DOWN;
    button[C_LEFT].button = SDLK_LEFT;
    button[C_RIGHT].button = SDLK_RIGHT;
    button[C_BTNX].button = SDLK_PAGEDOWN;      // button X
    button[C_BTNY].button = SDLK_PAGEUP;        // button Y
    button[C_BTNA].button = SDLK_HOME;          //button A
    button[C_BTNB].button = SDLK_END;           //button B
    button[C_SHOULDERA].button = SDLK_RSHIFT;   //L-Shoulder
    button[C_SHOULDERB].button = SDLK_RCTRL;    //R-Shoulder
    button[C_BTNMENU].button = SDLK_LALT;       //button START
    button[C_BTNSELECT].button = SDLK_LCTRL;    //button SELECT
    // vol button not used
  #else
    button[C_UP].button = SDLK_UP;
    button[C_DOWN].button = SDLK_DOWN;
    button[C_LEFT].button = SDLK_LEFT;
    button[C_RIGHT].button = SDLK_RIGHT;
    button[C_BTNY].button = SDLK_z;
    button[C_BTNX].button = SDLK_x;
    button[C_BTNA].button = SDLK_LALT;
    button[C_BTNB].button = SDLK_LCTRL;
    button[C_SHOULDERA].button = SDLK_1;
    button[C_SHOULDERB].button = SDLK_2;
    button[C_BTNMENU].button = SDLK_ESCAPE;
    button[C_BTNSELECT].button = SDLK_SPACE;
    button[C_BTNVOLUP].button = SDLK_F1;
    button[C_BTNVOLDOWN].button = SDLK_F2;
  #endif

  memset( &inpPointer, 0, sizeof(inpPointerState_t) );
  inpPointer.timeSinceMoved=POINTER_SHOW_TIMEOUT;
}
