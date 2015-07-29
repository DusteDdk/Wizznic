#ifndef SETTINGS_HPP_INCLUDED
#define SETTINGS_HPP_INCLUDED

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

#define SETMASK_UPLOAD_ONLINE 1
#define SETMASK_UPLOAD_ENABLED 2
#define SETMASK_UPLOAD_FIRSTRUN 4
#include <SDL/SDL.h>

struct settings_s {
  int wizVol; //Only used on wiz.
  int soundVol; //
  int musicVol;//
  int showFps;
  int wizClock;//
  int arcadeMode;
  int particles;

  int bpp; //bit per pixel that the "screen" runs
  int glHeight, glWidth, glEnable,glFilter;
  int fullScreen;
  int rift;

  char* packDir; //
  char* playerName;
  char* musicDir;
  int userMusic; ///What is this for?
  int disableMusic; //no music at all

  int uploadStats; //Allow to connect to STATS_SERVER_URL
  int online; //are we online, should not be saved
  int firstRun; //Is 1 if there's no config file (only happens once, or if stuffs really buggy)
  int showWeb; //print curl/wget command lines (for debugging)

  SDL_Rect bgPos; //background position
  int solvedWorldWide;
  int session; //Session for uploadstats
  float scaleFactor; //Factor that a pixel is scaled
};
typedef struct settings_s settings_t;

settings_t* setting();
void saveSettings();
void loadSettings(); //Load (But don't apply) settings
void applySettings(); //Without loading first (as not to override unsaved settings)
void initSettings(); //Set ptrs to 0 and load

#endif // SETTINGS_HPP_INCLUDED
