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

#include "input.h"
#include "ticks.h"
#include "defs.h"
#include "platform/dumplevelimages.h"

#include "settings.h"

static SDLKey inputChar=0;
static int joyCanMoveX=0;
static int joyCanMoveY=0;

SDLKey getChar()
{
  return(inputChar);
}

void resetMouseBtn()
{
  getInpPointerState()->isDown=0;
  getInpPointerState()->downTime=1;
  getInpPointerState()->hitABox=0;
}

static struct {
  uint32_t button;
  int state;
  int time;
} button[C_NUM];

int getButton(int btn)
{
  return(button[btn].state);
}

void setButton(int btn)
{
  button[btn].state=1;
  button[btn].time=0;
}


void resetBtnTimer(int btn)
{
  button[btn].time=0;
}

void resetBtn(int btn)
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

int getBtnTime(int btn)
{
  return(button[btn].time);
}

int runControls()
{
  SDL_Event event;
  int i;
  inputChar=0;
  //Loop through buttons to update hold-down time
  for(i=0; i < C_NUM; i++)
  {
    if( button[i].state )
    {
      button[i].time += getTicks();
    }
  }

  if( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT)
    getInpPointerState()->timeSinceMoved +=getTicks();

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

	#else
	//Joystick
	case SDL_JOYAXISMOTION:	// Analog joystick movement
		switch(event.jaxis.axis)
		{
			case 0:
				if(event.jaxis.value < -PLATFORM_JOY_DEADZONE && joyCanMoveX)
				{
					joyCanMoveX = 0;
					// left movement
					button[C_LEFT].state=1;
					button[C_LEFT].time=0;
				}
				else if(event.jaxis.value > PLATFORM_JOY_DEADZONE && joyCanMoveX)
				{
					joyCanMoveX = 0;
					// right movement
					button[C_RIGHT].state=1;
					button[C_RIGHT].time=0;
				}
				else if(event.jaxis.value > -PLATFORM_JOY_DEADZONE && event.jaxis.value < PLATFORM_JOY_DEADZONE)
				{
					joyCanMoveX = 1;
					// is at center
					button[C_LEFT].state=0;
					button[C_LEFT].time=0;
					button[C_RIGHT].state=0;
					button[C_RIGHT].time=0;
				}
			break;
			case 1:
				if(event.jaxis.value < -PLATFORM_JOY_DEADZONE && joyCanMoveY)
				{
					joyCanMoveY = 0;
					// up movement
					button[C_UP].state=1;
					button[C_UP].time=0;
				}
				else if(event.jaxis.value > PLATFORM_JOY_DEADZONE && joyCanMoveY)
				{
					joyCanMoveY = 0;
					// down movement
					button[C_DOWN].state=1;
					button[C_DOWN].time=0;
				}
				else if(event.jaxis.value > -PLATFORM_JOY_DEADZONE && event.jaxis.value < PLATFORM_JOY_DEADZONE)
				{
					joyCanMoveY = 1;
					// is at center
					button[C_UP].state=0;
					button[C_UP].time=0;
					button[C_DOWN].state=0;
					button[C_DOWN].time=0;
				}
			break;

			default:
			break;
		}
	break;

	case SDL_JOYHATMOTION:
		button[C_UP].state=(event.jhat.value & SDL_HAT_UP);
		button[C_UP].time=0;
		button[C_DOWN].state=(event.jhat.value & SDL_HAT_DOWN);
		button[C_DOWN].time=0;
		button[C_LEFT].state=(event.jhat.value & SDL_HAT_LEFT);
		button[C_LEFT].time=0;
		button[C_RIGHT].state=(event.jhat.value & SDL_HAT_RIGHT);
		button[C_RIGHT].time=0;
	break;

	case SDL_JOYBUTTONDOWN:
		switch (event.jbutton.button)
		{
			case 0:
				button[C_BTNB].state = 1;
				button[C_BTNB].time=0;
			break;
			case 1:
				button[C_BTNX].state = 1;
				button[C_BTNX].time=0;
			break;
			case 2:
				button[C_BTNA].state = 1;
				button[C_BTNA].time=0;
			break;
			case 3:
				button[C_BTNB].state = 1;
				button[C_BTNB].time=0;
			break;
			case 4:
				button[C_BTNMENU].state = 1;
				button[C_BTNMENU].time=0;
			break;
			case 5:
				button[C_BTNSELECT].state = 1;
				button[C_BTNSELECT].time=0;
			break;
		}
	break;

	case SDL_JOYBUTTONUP:
		switch (event.jbutton.button)
		{
			case 0:
				button[C_BTNB].state = 0;
				button[C_BTNB].time=0;
			break;
			case 1:
				button[C_BTNX].state = 0;
				button[C_BTNX].time=0;
			break;
			case 2:
				button[C_BTNA].state = 0;
				button[C_BTNA].time=0;
			break;
			case 3:
				button[C_BTNB].state = 0;
				button[C_BTNB].time=0;
			break;
			case 4:
				button[C_BTNMENU].state = 0;
				button[C_BTNMENU].time=0;
			break;
			case 5:
				button[C_BTNSELECT].state = 0;
				button[C_BTNSELECT].time=0;
			break;
		}
	break;

        //Keyboard
        case SDL_KEYDOWN:
          for(i=0; i < C_NUM; i++)
          {
            if( event.key.keysym.sym == button[i].button )
            {
              button[i].state=1;
              button[i].time=0;
            }
          }

          if( event.key.keysym.sym > 31 && event.key.keysym.sym < 123 )
          {
            inputChar=event.key.keysym.sym;

            if( event.key.keysym.mod & KMOD_SHIFT )
            {
              inputChar=toupper(inputChar);
            }
          } else if( event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_RETURN )
          {
            inputChar=event.key.keysym.sym;
#if defined(PC)
          } else if( event.key.keysym.sym == SDLK_F1 )
          {
            screenShot();
#endif
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
          if( getInpPointerState()->curX > -1 && getInpPointerState()->curX< 11 &&
              getInpPointerState()->curY > -1 && getInpPointerState()->curY < 11 )
          {
            getInpPointerState()->downTime=0;
            getInpPointerState()->isDown=1;
            getInpPointerState()->hitABox=0; //Clear precvious state.
          }
        break;
        case SDL_MOUSEBUTTONUP:
          getInpPointerState()->isDown=0;
          getInpPointerState()->timeSinceMoved=1;
        break;
        case SDL_MOUSEMOTION:

          getInpPointerState()->vpX = (event.motion.x/setting()->scaleFactor);
          getInpPointerState()->vpY = (event.motion.y/setting()->scaleFactor);

          getInpPointerState()->curX = (getInpPointerState()->vpX-(boardOffsetX*setting()->scaleFactor))/(20);
          getInpPointerState()->curY = (getInpPointerState()->vpY-(boardOffsetY*setting()->scaleFactor))/(20);

          getInpPointerState()->timeSinceMoved=0;

          if( getInpPointerState()->curX < 0 ) getInpPointerState()->curX = 0;
          if( getInpPointerState()->curX > 10 ) getInpPointerState()->curX = 10;
          if( getInpPointerState()->curY < 0 ) getInpPointerState()->curY = 0;
          if( getInpPointerState()->curY > 10 ) getInpPointerState()->curY = 10;
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
}


