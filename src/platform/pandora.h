#ifndef PANDORA_H_INCLUDED
#define PANDORA_H_INCLUDED


#define SCREENW 320
#define SCREENH 240


//For Uploading stats and downloading DLC's
#define PLATFORM_SUPPORTS_STATSUPLOAD

#define CMD_UPLOAD_STATS_POST "curl --user-agent wizznicPandoraCurl --connect-timeout 10 --fail --silent --url "API_URL"/stats/commit --data-ascii "
#define CMD_CHECK_DLC_API_VERSION "curl --user-agent wizznicPandoraCurl --connect-timeout 10 --fail --silent --url "API_URL"/check/version"
#define CMD_DOWNLOAD_DLC_FILE "curl --user-agent wizznicPandoraCurl --connect-timeout 10 --fail --silent --url "API_URL"/packs/get/%s -o %s"
#define CMD_UPLOAD_DLC_FILE "curl --user-agent wizznicPandoraCurl --connect-timeout 10 --fail --silent --url "API_URL"/packs/add/level/ --data-binary \"@%s\""


//Audio
#define SOUND_RATE  22050
#define SOUND_FORMAT  AUDIO_S16
#define SOUND_BUFFERS 256
#define SOUND_MIX_CHANNELS 16

#define PLATFORM_HELP_FILE        "data/menu/helppandora.png"

//Button definitions
#define PLATFORM_BUTTON_UP        SDLK_UP
#define PLATFORM_BUTTON_DOWN      SDLK_DOWN
#define PLATFORM_BUTTON_LEFT      SDLK_LEFT
#define PLATFORM_BUTTON_RIGHT     SDLK_RIGHT
#define PLATFORM_BUTTON_Y         SDLK_PAGEUP
#define PLATFORM_BUTTON_X         SDLK_PAGEDOWN
#define PLATFORM_BUTTON_A         SDLK_HOME
#define PLATFORM_BUTTON_B         SDLK_END
#define PLATFORM_SHOULDER_LEFT    SDLK_RSHIFT
#define PLATFORM_SHOULLER_RIGHT   SDLK_RCTRL
#define PLATFORM_BUTTON_MENU      SDLK_LALT
#define PLATFORM_BUTTON_SELECT    SDLK_LCTRL
#define PLATFORM_BUTTON_VOLUP     0 //Not used
#define PLATFORM_BUTTON_VOLDOWN   0 //on Pandora

#endif // PANDORA_H_INCLUDED
