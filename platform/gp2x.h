#ifndef _GP2X_H
#define _GP2X_H

/* This file was copied with little or no modification from the WizPort of OpenJazz */


//Video
#define SCREENW 320
#define SCREENH 240

//Sound
#define SOUND_RATE  22050
#define SOUND_FORMAT  AUDIO_S16
#define SOUND_BUFFERS 256
#define SOUND_MIX_CHANNELS 8


#define VOLUME_MIN 0
#define VOLUME_MAX 100
#define VOLUME_CHANGE_RATE 4
#define VOLUME_NOCHG 0
#define VOLUME_DOWN 1
#define VOLUME_UP 2

#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (14)
#define GP2X_BUTTON_Y               (15)
#define GP2X_BUTTON_L               (10)
#define GP2X_BUTTON_R               (11)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

//Help file for this platform (appended to DATADIR)
#define PLATFORM_HELP_FILE        "data/menu/helpwiz.png"

//Button definitions
#define PLATFORM_BUTTON_UP        GP2X_BUTTON_UP
#define PLATFORM_BUTTON_DOWN      GP2X_BUTTON_DOWN
#define PLATFORM_BUTTON_LEFT      GP2X_BUTTON_LEFT
#define PLATFORM_BUTTON_RIGHT     GP2X_BUTTON_RIGHT
#define PLATFORM_BUTTON_X         GP2X_BUTTON_X
#define PLATFORM_BUTTON_Y         GP2X_BUTTON_Y
#define PLATFORM_BUTTON_A         GP2X_BUTTON_A
#define PLATFORM_BUTTON_B         GP2X_BUTTON_B
#define PLATFORM_SHOULDER_LEFT    GP2X_BUTTON_L
#define PLATFORM_SHOULLER_RIGHT   GP2X_BUTTON_R
#define PLATFORM_BUTTON_MENU      GP2X_BUTTON_START
#define PLATFORM_BUTTON_SELECT    GP2X_BUTTON_SELECT
#define PLATFORM_BUTTON_VOLUP     GP2X_BUTTON_VOLUP
#define PLATFORM_BUTTON_VOLDOWN   GP2X_BUTTON_VOLDOWN

#include "../text.h"
void WIZ_ShowVolume(SDL_Surface* screen);
void WIZ_SetVolume( int volumePercent );
int WIZ_GetVolume();
void WIZ_AdjustVolume( int direction );

#endif
