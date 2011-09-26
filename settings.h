#ifndef SETTINGS_HPP_INCLUDED
#define SETTINGS_HPP_INCLUDED

#define SETMASK_UPLOAD_ONLINE 1
#define SETMASK_UPLOAD_ENABLED 2
#define SETMASK_UPLOAD_FIRSTRUN 4
#include <SDL/SDL.h>

struct settings_s {
  int wizVol; //Only used in wiz.
  int soundVol; //
  int musicVol;//
  int showFps;
  int wizClock;//
  int arcadeMode;
  int particles;

  char* packDir; //
  char* playerName;
  char* musicDir;
  int userMusic; ///What is this for?
  int disableMusic; //no music at all

  int uploadStats; //Allow to connect to STATS_SERVER_URL
  int online; //are we online, should not be saved
  int firstRun; //Is 1 if there's no config file (only happens once, or if stuffs really buggy)

  SDL_Rect bgPos; //background position
};
typedef struct settings_s settings_t;

inline settings_t* setting();
void saveSettings();
void loadSettings(); //Load (But don't apply) settings
void applySettings(); //Without loading first (as not to override unsaved settings)
void initSettings(); //Set ptrs to 0 and load

#endif // SETTINGS_HPP_INCLUDED
