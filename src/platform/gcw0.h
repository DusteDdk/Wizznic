#ifndef GCW0_H_INCLUDED
#define GCW0_H_INCLUDED

//Stats uploading for GCW Zero, using wget ?
#define PLATFORM_SUPPORTS_STATSUPLOAD
#define STR_PLATFORM "GCW Zero"
#if defined(WITH_CURL)
  #define CMD_UPLOAD_STATS_POST "curl --user-agent wizznicGCW0Curl --connect-timeout 10 --fail --silent --url "API_URL"/stats/commit --data-ascii "
  #define CMD_CHECK_DLC_API_VERSION "curl --user-agent wizznicGCW0Curl --connect-timeout 10 --fail --silent --url "API_URL"/check/version"
  #define CMD_DOWNLOAD_DLC_FILE "curl --user-agent wizznicGCW0Curl --connect-timeout 10 --fail --silent --url "API_URL"/packs/get/%s -o %s"
  #define CMD_UPLOAD_DLC_FILE "curl --user-agent wizznicGCW0Curl --connect-timeout 10 --fail --silent --url "API_URL"/packs/add/level/ --data-binary \"@%s\""
#else
  #define CMD_UPLOAD_STATS_POST "wget "API_URL"/stats/commit -O - -q --user-agent=wizznicGCW0Wget --timeout=10 --tries=1 --post-data="
  #define CMD_CHECK_DLC_API_VERSION "wget -q --user-agent=wizznicGCW0Wget --timeout=10 --tries=1 -O - "API_URL"/check/version"
  #define CMD_DOWNLOAD_DLC_FILE "wget -q --user-agent=wizznicGCW0Wget --timeout=10 --tries=1 "API_URL"/packs/get/%s -O %s"
  #define CMD_UPLOAD_DLC_FILE "wget -q --user-agent=wizznicGCW0Wget --timeout=10 --tries=1 --post-file \"%s\" -O - "API_URL"/packs/add/level/"

#endif

//Video
#define SCREENW 320
#define SCREENH 240

//Audio
#define SOUND_RATE  22050
#define SOUND_FORMAT  AUDIO_S16
#define SOUND_BUFFERS 256
#define SOUND_MIX_CHANNELS 16

//Help file for this platform (appended to DATADIR)
#define PLATFORM_HELP_FILE        "data/menu/helpgcw0.png"

//Button definitions
#define PLATFORM_BUTTON_UP        SDLK_UP
#define PLATFORM_BUTTON_DOWN      SDLK_DOWN
#define PLATFORM_BUTTON_LEFT      SDLK_LEFT
#define PLATFORM_BUTTON_RIGHT     SDLK_RIGHT
#define PLATFORM_BUTTON_Y         SDLK_SPACE
#define PLATFORM_BUTTON_X         SDLK_LALT
#define PLATFORM_BUTTON_A         SDLK_LSHIFT
#define PLATFORM_BUTTON_B         SDLK_LCTRL
#define PLATFORM_SHOULDER_LEFT    SDLK_TAB
#define PLATFORM_SHOULLER_RIGHT   SDLK_BACKSPACE
#define PLATFORM_BUTTON_MENU      SDLK_RETURN
#define PLATFORM_BUTTON_SELECT    SDLK_ESCAPE
#define PLATFORM_BUTTON_VOLUP     0 //Not used
#define PLATFORM_BUTTON_VOLDOWN   0 //on GCW Zero
#define PLATFORM_JOY_DEADZONE	  4000

#endif // GCW0_H_INCLUDED
