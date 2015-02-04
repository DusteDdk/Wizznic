#ifndef PC_H_INCLUDED
#define PC_H_INCLUDED

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

#include "dumplevelimages.h"

#define SCREENW 320
#define SCREENH 240

//For Uploading stats and downloading DLC's
#define PLATFORM_SUPPORTS_STATSUPLOAD

#if defined(WIN32)
  #if defined(WITH_OPENGL)
    #define GLEW_STATIC
    #include <GL/glew.h>
  #endif

  #define STR_PLATFORM "Windows"
  #define CMD_UPLOAD_STATS_POST "curl\\curl.exe --user-agent wizznicWindowsCurl --connect-timeout 10 --fail --silent --url "STATS_SERVER_URL"commit/ --data-ascii "
  #define CMD_CHECK_DLC_API_VERSION "curl\\curl.exe --user-agent wizznicWindowsCurl --connect-timeout 10 --fail --silent  --url "DLC_SERVER_URL"check/version"
  #define CMD_DOWNLOAD_DLC_FILE "curl\\curl.exe --user-agent wizznicWindowsCurl --connect-timeout 10 --fail --silent --url "DLC_SERVER_URL"get/%s -o %s"
#elif defined(linux) || defined(__linux)
  #define STR_PLATFORM "Linux"
  #if defined(WITH_CURL)
    #define CMD_UPLOAD_STATS_POST "curl --user-agent wizznicLinuxCurl --connect-timeout 10 --fail --silent --url "STATS_SERVER_URL"commit/ --data-ascii "
    #define CMD_CHECK_DLC_API_VERSION "curl --user-agent wizznicLinuxCurl --connect-timeout 10 --fail --silent --url "DLC_SERVER_URL"check/version"
    #define CMD_DOWNLOAD_DLC_FILE "curl --user-agent wizznicLinuxCurl --connect-timeout 10 --fail --silent --url "DLC_SERVER_URL"get/%s -o %s"
  #else
    #define CMD_UPLOAD_STATS_POST "wget "STATS_SERVER_URL"/commit/ -O - -q --user-agent=wizznicLinuxWget --timeout=10 --tries=1 --post-data="
    #define CMD_CHECK_DLC_API_VERSION "wget -q --user-agent=wizznicLinuxWget --timeout=10 --tries=1 -O - "DLC_SERVER_URL"check/version"
    #define CMD_DOWNLOAD_DLC_FILE "wget -q --user-agent=wizznicLinuxWget --timeout=10 --tries=1 "DLC_SERVER_URL"get/%s -O %s"
  #endif
#endif

#ifndef WIN32
  #if defined(WITH_OPENGL)
    #include <GL/gl.h>
    #include <GL/glu.h>
  #endif
#endif

//Sound defines
#define SOUND_RATE  44100
#define SOUND_FORMAT  AUDIO_S16
#define SOUND_BUFFERS 2048
#define SOUND_MIX_CHANNELS 16

//Button definitions
#if defined (MAME_CTRL) //For MAME cabinet
  #define PLATFORM_HELP_FILE        "data/menu/helppc-mame.png"
  #define PLATFORM_BUTTON_UP        SDLK_UP
  #define PLATFORM_BUTTON_DOWN      SDLK_DOWN
  #define PLATFORM_BUTTON_LEFT      SDLK_LEFT
  #define PLATFORM_BUTTON_RIGHT     SDLK_RIGHT
  #define PLATFORM_BUTTON_X         SDLK_SPACE
  #define PLATFORM_BUTTON_Y         SDLK_LSHIFT
  #define PLATFORM_BUTTON_A         SDLK_LALT
  #define PLATFORM_BUTTON_B         SDLK_LCTRL
  #define PLATFORM_BUTTON_MENU      SDLK_1
  #define PLATFORM_BUTTON_SELECT    SDLK_2
  #define PLATFORM_BUTTON_VOLUP     SDLK_a
  #define PLATFORM_BUTTON_VOLDOWN   SDLK_s
  #define PLATFORM_SHOULDER_LEFT    SDLK_8
  #define PLATFORM_SHOULLER_RIGHT   SDLK_9
#else //Normal PC keyboard layout.
  #define PLATFORM_HELP_FILE        "data/menu/helppc.png"
  #define PLATFORM_BUTTON_UP        SDLK_UP
  #define PLATFORM_BUTTON_DOWN      SDLK_DOWN
  #define PLATFORM_BUTTON_LEFT      SDLK_LEFT
  #define PLATFORM_BUTTON_RIGHT     SDLK_RIGHT
  #define PLATFORM_BUTTON_X         SDLK_x
  #define PLATFORM_BUTTON_Y         SDLK_z
  #define PLATFORM_BUTTON_A         SDLK_LALT
  #define PLATFORM_BUTTON_B         SDLK_LCTRL
  #define PLATFORM_SHOULDER_LEFT    SDLK_1
  #define PLATFORM_SHOULLER_RIGHT   SDLK_2
  #define PLATFORM_BUTTON_MENU      SDLK_ESCAPE
  #define PLATFORM_BUTTON_SELECT    SDLK_SPACE
  #define PLATFORM_BUTTON_VOLUP     SDLK_F1
  #define PLATFORM_BUTTON_VOLDOWN   SDLK_F2
#endif

//Accelerated scaling
#if defined(WITH_OPENGL)
  #include <SDL/SDL.h>
  #define HAVE_ACCELERATION
  SDL_Surface* platformInitAccel( int sdlVideoModeFlags );
  void platformDrawScaled(SDL_Surface* src);
#endif

//PC's are always able to use sw-scaling
#define WANT_SWSCALE

#endif // PC_H_INCLUDED
