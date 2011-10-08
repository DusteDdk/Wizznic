#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

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

#if !defined (GP2X) && !defined (PSP)
  #include "dumplevelimages.h"
#endif

#ifndef DATADIR
  #define DATADIR ""
  #error "DATADIR not defined."
#endif

#ifdef PSP
  #define MAJOR_VERSION  1
  #define MINOR_VERSION  0
  PSP_MODULE_INFO("Wizznic", 0, MAJOR_VERSION, MINOR_VERSION);
  PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
  PSP_HEAP_SIZE_KB(-256);
  #define printf pspDebugScreenPrintf
#endif

int main(int argc, char *argv[])
{
  int doScale=0;
  int doDump=0;
  int doThumb=0;
  int state=1; //Game, Menu, Editor, Quit
  int sdlVideoModeFlags = SDL_SWSURFACE;

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
  printf( "Wizznic "VERSION_STRING". GPLv3 or newer Copyleft 2010\n\n");

  //initialize path strings
  initUserPaths();

  //Tell where stuff's at.
  printf("Directories:\n    Settings: %s\n    Highscores: %s\n    Editorlevels: %s\n    Datafiles: %s\n\n", \
                            getConfigDir(), getHighscoreDir(), getUserLevelDir(), (!strlen(DATADIR))?".":DATADIR);

  printf("Loading settings...\n");
  //Read settings
  initSettings();

  atexit(SDL_Quit);

  //Init SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER ) <0 )
  {
    printf("SDL_Init failed: %s\n",SDL_GetError());
    return(-1);
  }

  //Setup display
  #if defined (GP2X) || defined (PSP)
  SDL_Surface* screen = SDL_SetVideoMode(SCREENW,SCREENH,16, sdlVideoModeFlags);
  #else
  SDL_Surface* scale;
  SDL_Surface* screen;

  int sdlFullScrFlag=0;
  if(argc==2 || argc==3 || argc==4)
  {
    if(strcmp(argv[1], "-z")==0)
    {
      doScale=2;
      if( argc==3 && atoi(argv[2])>0 && atoi(argv[2]) < 20 )
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
      doDump=1;
    } else if(strcmp(argv[1], "-thumbnailer")==0 && argc==4)
    {
      doThumb=1;
    } else if(strcmp(argv[1], "-f")==0)
    {
      sdlFullScrFlag=SDL_FULLSCREEN;
    } else if(!doScale)
    {
      printf("Usage:\n wizznic -d PACKNAME Dumps levelimages for pack.\n wizznic -z [n] Zoom to 320*n x 240*n\nwizznic -f run 320x240 in fullscreen\n wizznic -thumbnailer LVLFILE OUTFILE\n");
      return(-1);
    }

  }

  if(doScale)
  {
    scale = SDL_SetVideoMode(SCREENW*doScale,SCREENH*doScale,16, sdlVideoModeFlags | sdlFullScrFlag);
    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320,240,16, scale->format->Rmask,scale->format->Gmask,scale->format->Bmask,0xff000000);
  } else {
    scale = SDL_SetVideoMode(SCREENW,SCREENH,16, SDL_SWSURFACE | sdlFullScrFlag);
    screen=scale;
  }

  //Set window title
  SDL_WM_SetCaption("Wizznic!", "Wizznic!");
  //Set window icon
  SDL_Surface* icon = IMG_Load( DATADIR"data/wmicon.png");
  SDL_WM_SetIcon(icon, NULL);
  SDL_FreeSurface(icon);

  #endif
  //Open Joysticks (for wiz)
  if (SDL_NumJoysticks() > 0) SDL_JoystickOpen(0);

  //Hide mousecursor (both PC and wiz needs this)
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

  #if !defined (GP2X) && !defined (PSP)
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

  printf("Applying settings..\n");
  //Apply settings (has to be done after packs are inited)
  applySettings();
  //Set Pack
  packSetByPath( setting()->packDir );


  #if !defined (GP2X) && !defined(PSP)
  if( (setting()->uploadStats) && !(setting()->firstRun) )
  {
    statsUpload(0,0,0,0,0,"check",1);
  }
  #endif

  printf("Setting Music...\n");
  //Start playing music (has to be done after readong settings)
  soundSetMusic();

  printf("Initializing Credits...\n");
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

    soundRun(screen,state);

    if(setting()->showFps)
      drawFPS(screen);

    #if defined (GP2X) || defined (PSP)
    while(SDL_GetTicks()-lastTick < 20)
    {

    }

    SDL_Flip(screen);

    #else
    //The pixel plotting seems to run faster than code usind SDL_Rect, so we still use that for 2x zoom.
    if(doScale==2)
    {
      int x,xx,y,yy;
      for(y=0; y< SCREENH; y++)
      {
        for(x=0; x < SCREENW; x++)
        {
          //readPixel(screen, x,y, &r,&g,&b);
          uint16_t c = freadPixel(screen,x,y);/*SDL_MapRGB(scale->format,r,g,b);*/
          xx=x*2;
          yy=y*2;
          plotPixelu(scale, xx,yy, c);
          plotPixelu(scale, xx+1,yy, c);
          plotPixelu(scale, xx,yy+1, c);
          plotPixelu(scale, xx+1,yy+1, c);
        }
      }
    } else if(doScale>2)
    {
      int x,y;
      SDL_Rect r;
      for(y=0; y< SCREENH; y++)
      {
        for(x=0; x < SCREENW; x++)
        {
          r.x=x*doScale;
          r.y=y*doScale;
          r.w=doScale;
          r.h=doScale;
          SDL_FillRect(scale, &r, freadPixel(screen,x,y));
        }
      }
    }

    SDL_Flip(scale);
    int t=SDL_GetTicks()-lastTick;
    if(t < 20)
    {
      SDL_Delay( 20 -t);
    }
    #endif

  }

  SDL_Quit();

  #ifdef GP2X
  WIZ_SetClock(533);
  #endif

  #ifdef PSP
  sceKernelExitGame();
  #endif

  //Not even trying to clean up, OS should do that anyway..
  return(0);
}


