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

#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#ifdef WIN32
  #include <direct.h>
  #define cwd _getcwd
#else
  #include <unistd.h>
  #define cwd getcwd
#endif

#include "settings.h"
#include "strings.h"
#include "pack.h"

#ifdef WIZ
  #include "platform/wiz.h"
#endif
#ifdef GP2X
  #include "platform/gp2x.h"
#endif

#include "sound.h"
#include "defs.h"
#include "userfiles.h"

static settings_t settings;

settings_t* setting()
{
  return(&settings);
}

//Sets ptrs 0 and load settings, only called once
void initSettings()
{
  settings.packDir=0;
  settings.playerName=0;
  printf( "Loading settings from: %s/settings.ini\n", getConfigDir() );

  loadSettings();
}

void loadSettings()
{
  char* buf = malloc(sizeof(char)*2048);
  char* set = malloc(sizeof(char)*1024);
  char* val = malloc(sizeof(char)*1024);

  settings.bgPos.x = HSCREENW-160;
  settings.bgPos.y = HSCREENH-120;

  //Set defaults
  settings.soundVol=64;
  settings.musicVol=72;
  settings.wizClock=450;
  settings.showFps=0;
  settings.arcadeMode=0;
  settings.particles=1;
  settings.userMusic=0;
  settings.disableMusic=0;
  settings.wizVol=52;
  settings.glWidth=-1;
  settings.glHeight=-1;
  settings.glEnable=1;
  settings.glFilter=0;
  settings.fullScreen=0;
  settings.showWeb=0;

  //Not written to file
  settings.rift=0;
  settings.scaleFactor=1.0;
  settings.session=0;
  settings.solvedWorldWide=0;
  settings.firstRun=1;
  settings.uploadStats=-1;
  settings.online=0; //This is what will be checked for in the rest of the code
                     //regardless of "uploadStats", it will only be 1 if
                     //"uploadStats" is 1, because the check that can set it 1 is only
                     //executed if it's enabled.


  //Free ptrs.
  if(settings.packDir)
    free(settings.packDir);

  if(settings.playerName)
    free(settings.playerName);

  settings.packDir = malloc(sizeof(char)*(strlen(DATADIR"packs/000_wizznic")+1) );
  strcpy(settings.packDir, DATADIR"packs/000_wizznic");

  settings.playerName = malloc(sizeof(char)*11 );
  strcpy(settings.playerName, "player");

  settings.musicDir = cwd( NULL, 0 );
  if( !settings.musicDir )
    printf("Out of memory, will crash soon.\n");

  sprintf( buf, "%s/settings.ini", getConfigDir() );

  FILE *f = fopen(buf, "r");
  if(f)
  {
    while( fgets(buf, 128, f) )
    {
      stripNewLine(buf);

      if(splitVals('=',buf, set, val))
      {
        if( strcmp("soundvol", set)==0 )
        {
          settings.soundVol = atoi(val);
        } else
        if( strcmp("musicvol", set)==0 )
        {
          settings.musicVol = atoi(val);
        } else
        if( strcmp("wizclock", set)==0 )
        {
          settings.wizClock = atoi(val);
        } else
        if( strcmp("wizvolume", set)==0 )
        {
          settings.wizVol = atoi(val);
        } else
        if( strcmp("showfps", set)==0 )
        {
          settings.showFps = atoi(val);
        } else
        if( strcmp("arcademode", set)==0 )
        {
          settings.arcadeMode = atoi(val);
        } else
        if( strcmp("particles", set)==0 )
        {
          settings.particles = atoi(val);
        } else
        if( strcmp("packdir", set)==0 )
        {
          free(settings.packDir);
          settings.packDir = malloc(sizeof(char)*(strlen(val)+1) );
          strcpy(settings.packDir,val);
        } else
        if( strcmp("playername", set)==0 )
        {
          if( strlen(set) < 11 )
          {
            strcpy(settings.playerName,val);
          } else {
            printf("Error, name: '%s' too long, max length is 10.\n",set);
          }
        } else
        if( strcmp("musicdir", set)==0 )
        {
          //We check if it starts with . it now has to be a full path.
          if( val[0] != '.' )
          {
            free(settings.musicDir);
            settings.musicDir = malloc(sizeof(char)*(strlen(val)+1) );
            strcpy(settings.musicDir, val);
          } else {
            printf("Using '%s' as music directory instead of '%s'.\n", settings.musicDir, val);
          }
        } else
        if( strcmp("usermusic", set)==0 )
        {
          settings.userMusic=atoi(val);
        } else
        if( strcmp("disablemusic",set)==0 )
        {
          settings.disableMusic=atoi(val);
        } else
        if( strcmp("allowonline",set)==0 )
        {
          //Only if the option is in the file, are we sure they had the choice.
          settings.uploadStats=atoi(val);
          if( settings.uploadStats!=-1 )
          {
            settings.firstRun=0;
          }
        } else
        if( strcmp("glwidth",set)==0 )
        {
          settings.glWidth=atoi(val);
        } else
        if( strcmp("glheight",set)==0 )
        {
          settings.glHeight=atoi(val);
        } else
        if( strcmp("glenable",set)==0 )
        {
          settings.glEnable=atoi(val);
        } else
        if( strcmp("glfilter",set)==0 )
        {
          settings.glFilter=atoi(val);
        } else
        if( strcmp("fullscreen",set)==0 )
        {
          settings.fullScreen=atoi(val);
        } else
        if( strcmp("showweb",set)==0 )
        {
          settings.showWeb=atoi(val);
        }


      }
    }
    //Close file
    fclose( f );
  }

  //Free the textbuffers
  free(buf);
  free(set);
  free(val);
}

void saveSettings()
{
  char* buf = malloc(sizeof(char)*1024);
  sprintf( buf, "%s/settings.ini", getConfigDir() );

  FILE *f = fopen(buf, "w");
  if(f)
  {
    fprintf(f, "# Sound volume (0-128)\nsoundvol=%i\n\n"
               "# Music volume (0-128)\nmusicvol=%i\n\n"
               "# For the GP2X Wiz handheld: CPU Mhz.\nwizclock=%i\n\n"
               "# For the GP2X Wiz handheld: System-volume.\nwizvolume=%i\n\n"
               "# Show the FPS counter? 0 = No,  1 = Yes.\nshowfps=%i\n\n"
               "# Use particle effects? 0 = No,  1 = Yes.\nparticles=%i\n\n"
               "# 0 = Normal mode, progress through levels.\n# 1 = Arcade mode: Start on first level at game-over.\narcademode=%i\n\n"
               "# The currently selected content pack.\npackdir=%s\n\n"
               "# Name of the player.\nplayername=%s\n\n"
               "# If usermusic is on, play music from this directory.\nmusicdir=%s\n\n"
               "# 0 = Play the music that comes with the game. 1 = Use user-selected music. \nusermusic=%i\n\n"
               "# 0 = Play music. 1 = Don't load any music (faster loading)\ndisablemusic=%i\n\n"
               "# Allow Wizznic to access the Internet (http://wizznic.org)\n# 0 = No, 1 = Yes.\n# Helps Jimmy create more balanced gameplay in future versions.\n# Enables DLC downloading.\nallowonline=%i\n\n"
               "# 0 = No OpenGL Scaling. 1 = Use OpenGL to scale the image.\nglenable=%i\n\n"
               "# How to scale graphics ( 0 = Blocky/Sharp, 1 = Smooth/Blurry)\nglfilter=%i\n\n"
               "# If using glenable, the width of the window in pixels.\nglwidth=%i\n\n"
               "# If using glenable, the height of the window in pixels.\n# If this is -1, then select window size automatically.\nglheight=%i\n\n"
               "# Go to full-screen mode.\nfullscreen=%i\n\n"
               "# printf network traffic (when uploadstats=1)\nshowweb=%i\n",
               settings.soundVol,
               settings.musicVol,
               settings.wizClock,
               settings.wizVol,
               settings.showFps,
               settings.particles,
               settings.arcadeMode,
               settings.packDir,
               settings.playerName,
               settings.musicDir,
               settings.userMusic,
               settings.disableMusic,
               settings.uploadStats,
               settings.glEnable,
               settings.glFilter,
               settings.glWidth,
               settings.glHeight,
               settings.fullScreen,
               settings.showWeb);
    fclose( f );
  } else {
    printf("saveSettings(); Error: Couldn't open 'settings.ini' for writing.\n");
  }
  free(buf);
}

void applySettings()
{
  //Set sound volume
  Mix_Volume(-1, setting()->soundVol );

  //Set music volume
  soundSetMusVol( setting()->musicVol );

  ///TODO: These should be "Platform_*"
  #if defined (GP2X)
  WIZ_SetVolume( settings.wizVol );
  #endif
  #if defined (WIZ)
  WIZ_SetVolume( settings.wizVol );
  WIZ_SetClock( setting()->wizClock );
  #endif
}
