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
  #include <math.h>
  #include "dumplevelimages.h"
#endif

#ifndef DATADIR
  #define DATADIR "."
  #warning "DATADIR was not defined. Using ./"
#endif

#ifdef PSP
  #define MAJOR_VERSION  1
  #define MINOR_VERSION  0
  PSP_MODULE_INFO("Wizznic", 0, MAJOR_VERSION, MINOR_VERSION);
  PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
  PSP_HEAP_SIZE_KB(-256);
  #define printf pspDebugScreenPrintf
#endif

#if defined(WITH_OPENGL)
  #ifdef WIN32
    #define GLEW_STATIC
    #define WINDOWS_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <GL/glew.h>
  #else
    #include <GL/gl.h>
    #include <GL/glu.h>
  #endif
  GLuint texture;
  GLuint dlist;
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
  SDL_Surface* scale=0;
  SDL_Surface* screen=0;

  int sdlFullScrFlag=0;
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
      sdlFullScrFlag=SDL_FULLSCREEN;
    } else if(!doScale)
    {
      printf("\n\nUsage:\n  wizznic -d PACKNAME Dumps levelimages for pack.\n  wizznic -z [n] Zoom to 320*n x 240*n\n  wizznic -f run 320x240 in fullscreen\n  wizznic -thumbnailer LVLFILE OUTFILE\n");
      return(-1);
    }

  }

  #if defined(WITH_OPENGL)
  if( setting()->glEnable && doScale==0 )
    doScale=-1;
  #endif

  if(doScale)
  {

    //OpenGL scaling
    if( doScale == -1 )
    {
    #if defined(WITH_OPENGL)
      const SDL_VideoInfo* vidinfo = SDL_GetVideoInfo();
      int w=setting()->glWidth,h=setting()->glHeight;
      if(sdlFullScrFlag==SDL_FULLSCREEN)
      {
        w = vidinfo->current_w;
        h = vidinfo->current_h;
      } else {
        //Find largest resolution within screen
        if(w==-1||h==-1)
        {
          int factor=(int)floor( (float)(vidinfo->current_h-1)/240.0 );
          w=320*factor;
          h=240*factor;
        }
      }
      scale = SDL_SetVideoMode(w,h,32, SDL_OPENGL | sdlVideoModeFlags | sdlFullScrFlag);
      screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320,240,24, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);

      int vW = (GLint)h*(320.0f/240.0f);

      glViewport(w/2-vW/2, 0, vW, (GLint)h);

      glClearColor(1,0,0,1);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho( 0, SCREENW, SCREENH, 0, 0,1);
      glColor4f(1,1,1,1);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glDisable(GL_DEPTH_TEST);
      glDisable( GL_CULL_FACE );
      glDisable(GL_LIGHTING);

      glEnable(GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glColor4f(1,1,1,1);


      glEnable(GL_TEXTURE_2D);
      glGenTextures( 1, &texture );
      glBindTexture( GL_TEXTURE_2D, texture );

      if( setting()->glFilter )
      {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      } else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }

      dlist = glGenLists (1);
      glNewList(dlist, GL_COMPILE);
      glBegin( GL_QUADS );
        glTexCoord2f(0,0);
        glVertex2i(0,0);
        glTexCoord2f(1,0);
        glVertex2i(320,0);
        glTexCoord2f(1,1);
        glVertex2i(320,240);
        glTexCoord2f(0,1);
        glVertex2i(0,240);
      glEnd();
      glEndList();


    #else
      printf("\nError:\nNo OpenGL support, recompile with -dWITH_GL or change scale setting.\nExiting...\n");
      return(-1);
    #endif
    } else if( doScale > 0 )
    {
      //Software scaling
      scale = SDL_SetVideoMode(SCREENW*doScale,SCREENH*doScale,16, sdlVideoModeFlags | sdlFullScrFlag);
      screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320,240,16, scale->format->Rmask,scale->format->Gmask,scale->format->Bmask,0xff000000);
    }
  } else {
    //No scaling (scale is the buffer flipped to hardware so we simply make them the same)
    scale = SDL_SetVideoMode(SCREENW,SCREENH,16, SDL_SWSURFACE | sdlFullScrFlag);
    screen=scale;
  }

  //Set scaling
  setting()->scaleFactor= (float)scale->h/240.0;
  printf("Scaling factor: %f\n", setting()->scaleFactor);

  //Set window title
  SDL_WM_SetCaption("Wizznic!", "Wizznic!");
  //Set window icon
  SDL_Surface* icon = IMG_Load( DATADIR"data/wmicon.png");
  SDL_WM_SetIcon(icon, NULL);
  SDL_FreeSurface(icon);

  #endif //< PC version

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
    statsUpload(0,0,0,0,0,"check",1, &(setting()->session) );
    statsUpload(0,0,0,0,0,"q_solved",1, &(setting()->solvedWorldWide) );
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

    //OpenGL scaling, scale is the screen and not used.
    #if defined(WITH_OPENGL)
    if( doScale==-1 )
    {
      glTexImage2D( GL_TEXTURE_2D, 0, screen->format->BytesPerPixel, screen->w, screen->h, 0, GL_BGR, GL_UNSIGNED_BYTE, screen->pixels );
      glCallList(dlist);
      SDL_GL_SwapBuffers();
    } else
    #endif
    //The pixel plotting seems to run faster than code usind SDL_Rect, so we still use that for 2x zoom.
    if(doScale==2)
    {
      int x,xx,y,yy;
      for(y=0; y< SCREENH; y++)
      {
        for(x=0; x < SCREENW; x++)
        {
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

    if( doScale > -1 )
    {
      SDL_Flip(scale);
    }



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


