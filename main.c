/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2013 Jimmy Christensen <dusted@dusted.dk>             *
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
#include "transition.h"

#include "bundle.h"
#include "libDLC.h"

#ifdef PC
  #include "dumplevelimages.h"
#endif

SDL_Surface* swScreen(int sdlVideoModeFlags)
{
  SDL_Surface* screen=SDL_SetVideoMode(SCREENW,SCREENH,16, SDL_SWSURFACE | sdlVideoModeFlags);

  if( !screen )
  {
    printf("\nERROR: Couldn't create the window, exiting...\n");
    return(NULL);
  } else {
    setting()->glEnable=0;
    printf("Videomode: Unscaled %ix%i@16 bits.\n",SCREENW, SCREENH);
  }

  return(screen);
}

int main(int argc, char *argv[])
{
  int doScale=0; // 0=Undefined, 1=320x240, -1=OpenGL, >1=SwScale
  char* dumpPack=NULL;
  int state=1; //Game, Menu, Editor, Quit
  int sdlVideoModeFlags = SDL_SWSURFACE;

  #ifdef PSP
    //Note to PSP porter, please test if HW is actually faster, Wizznic does a lot of memory-manipulation in the screen-surface, each call might initiate a full copy back/forth from video memory. Remove comment when read. :)
    sdlVideoModeFlags = (SDL_HWSURFACE | SDL_DOUBLEBUF |SDL_HWACCEL);
    SetupCallbacks();//Callbacks actifs
    scePowerSetClockFrequency(333,333,166);
  #endif

//  #ifdef WIN32
//Redirect stdout to console on windows, so we can see what's going in.
//  FILE *stream;
//  stream = freopen("CON", "w", stdout);
//  #endif

  //Print welcome message
  printf( "Wizznic "VERSION_STRING". GPLv3 or newer Copyleft 2010-2013\n\n");

  //initialize path strings
  initUserPaths();

  //Tell where stuff's at.
  printf("Directories:\n    Settings: %s\n    DLC: %s\n    Highscores: %s\n    Editorlevels: %s\n    Datafiles: %s\n\n", \
                            getConfigDir(), getUsrPackDir(), getHighscoreDir(), getUserLevelDir(), (!strlen(DATADIR))?".":DATADIR);

  //Print the command line parameters
  printf("Command-line parameters:\n"STR_VID_OPTIONS);

  //Quit if user wants help
  if( argc > 1 && ( strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0 || strcmp(argv[1], "-help")==0 ))
  {
    printf("Please see readme.txt or http://wizznic.org/ for more help.\n");
    return(0);
  }

  //Read settings
  printf("Loading settings...\n");
  initSettings();

  #if defined(WITH_OPENGL)
  //We start by enabling glScaling if it was enabled in settings, it can then be overwritten by command line options.
  if( setting()->glEnable && doScale==0 )
    doScale=-1;
  #endif

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

  int i;
  for( i=0; i < argc; i++ )
  {
    if( strcmp( argv[i], "-sw" ) == 0 )
    {
      setting()->glEnable=0;
      doScale=0;
      saveSettings();
    } else
    if( strcmp( argv[i], "-gl" ) == 0 )
    {
      setting()->glEnable=1;
      doScale=-1;
      saveSettings();
    } else
    if( strcmp( argv[i], "-z" ) == 0 )
    {
      if( i+1 < argc )
      {
        doScale = atoi( argv[i+1] );
        setting()->glEnable=0;
        i++;
        saveSettings();
      } else {
        printf(" -z requires zoom level ( -z 2 for example ).\n");
        return(1);
      }
    } else
    if( strcmp( argv[i], "-f" ) == 0 )
    {
        setting()->fullScreen=1;
        saveSettings();
    } else
    if( strcmp( argv[i], "-w" ) == 0 )
      {
        setting()->fullScreen=0;
        saveSettings();
    } else if( strcmp( argv[i], "-glheight" ) == 0 )
    {
      if( i+1 < argc )
      {
        setting()->glHeight = atoi( argv[i+1] );
        setting()->glEnable=1;
        doScale=-1;
        i++;
        printf("Setting OpenGL window height to %i.\n", setting()->glHeight);
        saveSettings();
      } else {
        printf(" -glheight requires an argument (-1 or size in pixels).\n");
        return(1);
      }
    } else if( strcmp( argv[i], "-glwidth" ) == 0 )
    {
      if( i+1 < argc )
      {
        setting()->glWidth = atoi( argv[i+1] );
        setting()->glEnable=1;
        doScale=-1;
        i++;
        printf("Setting OpenGL window width to %i.\n", setting()->glWidth);
        saveSettings();
      } else {
        printf(" -glwidth requires an argument (-1 or size in pixels).\n");
        return(1);
      }
    } else if( strcmp( argv[i], "-glfilter" ) == 0 )
    {
      if( i+1 < argc )
      {
        setting()->glFilter=atoi(argv[i+1]);
        printf("OpenGL texture filtering set to %s.\n", (setting()->glFilter)?"Smooth":"Off");
        i++;
        saveSettings();
      } else {
        printf("-glfilter requires 0 or 1 as argument.\n");
        return(1);
      }
    } else if( strcmp( argv[i] , "-d" ) == 0 )
    {
      if( argc == 3 && i < argc+1 )
      {
        dumpPack = malloc( sizeof(char)*strlen(argv[i+1])+1 );
        strcpy( dumpPack, argv[i+1] );
        doScale=0;
        setting()->glEnable=0;
        i++;
      } else {
        printf("-d requires a packname, and must not be used with other parameters.\n");
        return(1);
      }
    } else if( strcmp( argv[i], "-rift") == 0 )
    {
      setting()->glWidth = 1280;
      setting()->glHeight = 800;
      setting()->glEnable=1;
      setting()->rift=1;
      setting()->fullScreen=0;
      doScale=-1;
    } else if( i > 0 )
    {
      printf("\nError: Invalid argument '%s', quitting.\n", argv[i]);
      return(1);
    }

  }

  if( setting()->fullScreen )
  {
    sdlVideoModeFlags |= SDL_FULLSCREEN;
  }

  if(doScale)
  {
    //Hardware accelerated scaling
    if( doScale == -1 )
    {
    #ifdef HAVE_ACCELERATION
      printf("Enabling platform specific accelerated scaling.\n");
      screen = platformInitAccel(sdlVideoModeFlags);
      if( !screen )
      {
        printf("Failed to set platform accelerated scaling, falling back to software window.\n");
        screen=swScreen(SDL_SWSURFACE);
        doScale=0;
      }
    #else
      printf("\nError:\n  Not compiled with hardware-scaling support, don't give me -z -1\n  Exiting...\n");
      return(-1);
    #endif
    } else if( doScale > 0 )
    {
    #ifdef WANT_SWSCALE
      //Set up software scaling
      printf("Enabling slow software-based scaling to %ix%i.\n",320*doScale, 240*doScale);
      screen = swScaleInit(sdlVideoModeFlags,doScale);
    #else
      printf("\nError:\n  I don't support software scaling, don't give me any -z options\n  Exiting...\n");
      return(-1);
    #endif
    }
  } else {
    screen=swScreen(sdlVideoModeFlags);
    doScale=0;
  }

  printf("Scaling factor: %f\n", setting()->scaleFactor);

  if( screen == NULL )
  {
    printf("ERROR: Couldn't init video.\n");
    return(-1);
  }


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

  //Hide mouse cursor
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

  //Seed the pseudo random number generator (for particles 'n' stuff)
  srand( (int)time(NULL) );

  #if defined(PC)
  //Need to dump level-screenshots?
  if(dumpPack)
  {
    printf("Dumping level-images for pack: %s\n", dumpPack);
    dumplevelimages(screen, dumpPack, 0);
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

    //DLC only works when stats-uploading is enabled so we can use the same nag-screen.
    dlcCheckOnline();
  }
  #endif

  printf("Setting Music...\n");
  //Start playing music (has to be done after readong settings)
  soundSetMusic();

  //Initialize credits
  initCredits(screen);

  initTransition();


#if SCREENW != 320 || SCREENH != 240
  SDL_Rect *borderSrcRect = malloc(sizeof(SDL_Rect));
  SDL_Surface* border = loadImg( BORDER_IMAGE );
  if( border )
  {
    printf("Border image loaded.\n");
    borderSrcRect->x=(border->w-SCREENW)/2;
    borderSrcRect->y=(border->h-SCREENH)/2;
    borderSrcRect->w=SCREENW;
    borderSrcRect->h=SCREENH;
    SDL_BlitSurface( border, borderSrcRect, screen, NULL );
    SDL_FreeSurface(border);
  } else {
    printf("Could not load border image: %s\n", BORDER_IMAGE);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));
  }
  free(borderSrcRect);
  borderSrcRect=NULL;

#endif

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

    runTransition(screen);

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
      #else
      default:
        state=STATEQUIT;
      break;
      #endif
    }

    #if defined(CRUDE_TIMING)
    //Oh how I loathe this, is there no better way?
    while(SDL_GetTicks()-lastTick <= PLATFORM_CRUDE_TIMING_TICKS)
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

  return(0);
}


