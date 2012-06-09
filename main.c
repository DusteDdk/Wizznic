/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2012 Jimmy Christensen <dusted@dusted.dk>             *
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

#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "defs.h"
#include "board.h"
#include "cursor.h"
#include "draw.h"
#include "input.h"
#include "sprite.h"
#include "text.h"
#include "sound.h"
#include "states.h"
#include "game.h"
#include "levels.h"
#include "leveleditor.h"
#include "particles.h"
#include "settings.h"
#include "pack.h"
#include "stats.h"
#include "credits.h"
#include "userfiles.h"
#include "strings.h"
#include "swscale.h"
#include "pointer.h"

#ifdef PC
  #include "dumplevelimages.h"
#endif

int main(int argc, char *argv[])
{
  int doScale=0;
  int doDump=0;
  int doThumb=0;
  int state=1; //Game, Menu, Editor, Quit
  int sdlVideoModeFlags = SDL_SWSURFACE;

  #ifdef PANDORA
  doScale=2; //Turn on software scaling (This turns off opengl)
  #endif

  #ifdef PSP
    //Note to PSP porter, please test if HW is actually faster, Wizznic does a lot of memory-manipulation in the screen-surface, each call might initiate a full copy back/forth from video memory. Remove comment when read. :)
    sdlVideoModeFlags = (SDL_HWSURFACE | SDL_DOUBLEBUF |SDL_HWACCEL);
    SetupCallbacks();//Callbacks actifs
    scePowerSetClockFrequency(333,333,166);
  #endif

  #ifdef WIN32
  //Redirect stdout to console on windows, so we can see what's going in.
  FILE *stream;
  stream = freopen("CON", "w", stdout);
  #endif



  //Print welcome message
  printf( "Wizznic "VERSION_STRING". GPLv3 or newer Copyleft 2010-2012\n\n");

  //initialize path strings
  initUserPaths();

  //Tell where stuff's at.
  printf("Directories:\n    Settings: %s\n    Highscores: %s\n    Editorlevels: %s\n    Datafiles: %s\n\n", \
                            getConfigDir(), getHighscoreDir(), getUserLevelDir(), (!strlen(DATADIR))?".":DATADIR);

  printf("Loading settings...\n");
  //Read settings
  initSettings();

  //Set scaling
  setting()->scaleFactor=1.0;

  atexit(SDL_Quit);

  //Init SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER ) <0 )
  {
    printf("SDL_Init failed: %s\n",SDL_GetError());
    return(-1);
  }

  //Setup display
  #if defined (GP2X) || defined (PSP) || defined (WIZ)
  SDL_Surface* screen = SDL_SetVideoMode(SCREENW,SCREENH,16, sdlVideoModeFlags);
  #else
  SDL_Surface* screen=NULL;

  if(argc==2 || argc==3 || argc==4)
  {
    if(strcmp(argv[1], "-z")==0)
    {
      doScale=2;
      if( argc==3 && atoi(argv[2]) !=0 && atoi(argv[2]) < 20 )
      {
        doScale = atoi(argv[2]);
      }
    } else if(strcmp(argv[1], "-d")==0)
    {
      if(argc!=3)
      {
        printf("usage: -d PACKNAME\n");
        return(-1);
      }
      setting()->glEnable=0;
      doDump=1;
    } else if(strcmp(argv[1], "-thumbnailer")==0 && argc==4)
    {
      doThumb=1;
    } else if(strcmp(argv[1], "-f")==0)
    {
      sdlVideoModeFlags |= SDL_FULLSCREEN;
    } else if(!doScale)
    {
      printf("\n\nUsage:\n  wizznic -d PACKNAME Dumps levelimages for pack.\n  wizznic -z [n] Zoom to 320*n x 240*n\n  wizznic -f run 320x240 in fullscreen\n  wizznic -thumbnailer LVLFILE OUTFILE\n");
      return(-1);
    }

  }

  #if defined(WITH_OPENGL)
  //If we were started with no -z option, we check to see if gl scaling was enabled in settings.ini
  if( setting()->glEnable && doScale==0 )
    doScale=-1;
  #endif

  if(doScale)
  {
    //Hardware accelerated scaling
    if( doScale == -1 )
    {
    #ifdef HAVE_ACCELERATION
      screen = platformInitAccel(sdlVideoModeFlags);
    #else
      printf("\nError:\n  Not compiled with hardware-scaling support, don't give me -z -1\n  Exiting...\n");
      return(-1);
    #endif
    } else if( doScale > 0 )
    {
    #ifdef WANT_SWSCALE
      //Set up software scaling
      screen = swScaleInit(sdlVideoModeFlags,doScale);
    #else
      printf("\nError:\n  I don't support software scaling, don't give me any -z options\n  Exiting...\n");
      return(-1);
    #endif
    }
  } else {
    //No scaling (scale is the buffer flipped to hardware so we simply make them the same)
    screen=SDL_SetVideoMode(SCREENW,SCREENH,16, SDL_SWSURFACE | sdlVideoModeFlags);
  }

  printf("Scaling factor: %f\n", setting()->scaleFactor);


  //Set window title
  SDL_WM_SetCaption("Wizznic!", "Wizznic!");
  //Set window icon
  SDL_Surface* icon = IMG_Load( DATADIR"data/wmicon.png");
  SDL_WM_SetIcon(icon, NULL);
  SDL_FreeSurface(icon);

  #endif

  setting()->bpp = screen->format->BytesPerPixel;
  setAlphaCol( setting()->bpp );

  printf("Screen surface using %i bytes per pixel.\n",setting()->bpp);

  //Open Joysticks (for wiz)
  if (SDL_NumJoysticks() > 0) SDL_JoystickOpen(0);

  //Hide mousecursor
  SDL_ShowCursor(SDL_DISABLE);

  //Load fonts
  txtInit();

  //Load sounds
  if(!initSound())
  {
    printf("Couldn't init sound.\n");
    return(-1);
  }

  //Menu Graphics
  if(!initMenu(screen))
  {
    printf("Couldn't load menu graphics.\n");
    return(-1);
  }

  //Init controls
  initControls();

  //Init stats
  statsInit();

  //Init packs
  packInit();

  //Scan userlevels dir
  makeUserLevelList(screen);

  //Init particles
  initParticles(screen);

  #if defined(PC)
  //Need to dump level-screenshots?
  if(doDump)
  {
    dumplevelimages(screen, argv[2], 0);
    return(0);
  } else if(doThumb)  //For the gnome-thumbnailing script
  {
    thumblevelimage(screen, argv[2], argv[3]);
    return(0);
  }
  #endif

  //init starfield
  initStars(screen);

  //Init pointer
  initPointer(screen);

  printf("Applying settings..\n");
  //Apply settings (has to be done after packs are inited)
  applySettings();
  //Set Pack
  packSetByPath( setting()->packDir );

  #if defined( PLATFORM_SUPPORTS_STATSUPLOAD )
  if( (setting()->uploadStats) && !(setting()->firstRun) )
  {
    statsUpload(0,0,0,0,0,"check",1, &(setting()->session) );
    statsUpload(0,0,0,0,0,"q_solved",1, &(setting()->solvedWorldWide) );
  }
  #endif

  printf("Setting Music...\n");
  //Start playing music (has to be done after readong settings)
  soundSetMusic();

  //Initialize credits
  initCredits(screen);

  int lastTick;
  while(state!=STATEQUIT)
  {
    lastTick=SDL_GetTicks();

    frameStart();
    if(runControls()) state=STATEQUIT;

    switch(state)
    {
      case STATEPLAY:
        state = runGame(screen);
      break;

      case STATEMENU:
        state = runMenu(screen);
      break;

      case STATEEDIT:
        state=runEditor(screen);
      break;
    }

    drawPointer(screen);

    soundRun(screen,state);

    if(setting()->showFps)
      drawFPS(screen);

    switch( doScale )
    {
      #if defined(HAVE_ACCELERATION)
      case -1:
        platformDrawScaled(screen);
        break;
      #endif
      case 0:
        SDL_Flip(screen);
        break;
      #if defined(WANT_SWSCALE)
      default:
        swScale(screen,doScale);
        break;
      #endif
    }

    #if defined(CRUDE_TIMING)
    //Oh how I loathe this, is there no better way?
    while(SDL_GetTicks()-lastTick < 20)
    {
      //Burn, burn baby burn!
    }
    #else
    int t=SDL_GetTicks()-lastTick;
    if(t < 20)
    {
      SDL_Delay( 20 -t);
    }
    #endif
  }


  #if defined(PLATFORM_NEEDS_EXIT)
  platformExit();
  #endif

  SDL_Quit();

  //Not even trying to clean up, OS should do that anyway..
  return(0);
}


