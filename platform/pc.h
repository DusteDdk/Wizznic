#ifndef PC_H_INCLUDED
#define PC_H_INCLUDED

#define SCREENW 600
#define SCREENH 400

#include <math.h>

//For Uploading stats
#if defined(WIN32)
  #if defined(WITH_OPENGL)
    #define GLEW_STATIC
    #define WINDOWS_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <GL/glew.h>
  #endif

  #define STR_PLATFORM "Windows"
  #define UPLOAD_PROGRAM "curl\\curl.exe --user-agent wizznicWindows --connect-timeout 10 --fail --silent --url "STATS_SERVER_URL"/commit.php --data-ascii "
#elif defined(linux) || defined(__linux)
  #define STR_PLATFORM "Linux"
  #define UPLOAD_PROGRAM "wget "STATS_SERVER_URL"/commit.php -O - -q --user-agent=wizznicLinux --timeout=10 --tries=1 --post-data="
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
