#ifndef PSPSPEC_H
#define PSPSPEC_H

// Sony PSP specific values and combatibility functions

// LCD Native size is only a tiny amount larger than wizznic-native
// screen, so we offset eveything to the middle instead.
#define SCREENW 480
#define SCREENH 272

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <psptypes.h>
#include <pspctrl.h>
#include <psppower.h>
#include <pspmoduleinfo.h>

#define PSP_BUTTON_TRIANGLE 0
#define PSP_BUTTON_CIRCLE   1
#define PSP_BUTTON_CROSS    2
#define PSP_BUTTON_SQUARE   3
#define PSP_BUTTON_LTRIGGER 4
#define PSP_BUTTON_RTRIGGER 5
#define PSP_BUTTON_DOWN     6
#define PSP_BUTTON_LEFT     7
#define PSP_BUTTON_UP       8
#define PSP_BUTTON_RIGHT    9
#define PSP_BUTTON_SELECT   10
#define PSP_BUTTON_START    11
#define PSP_BUTTON_HOME     12
#define PSP_BUTTON_HOLD     13

//Help file for this platform (appended to DATADIR)
#define PLATFORM_HELP_FILE        "data/menu/helppsp.png"

//Button definitions
#define PLATFORM_BUTTON_UP        PSP_BUTTON_UP
#define PLATFORM_BUTTON_DOWN      PSP_BUTTON_DOWN
#define PLATFORM_BUTTON_LEFT      PSP_BUTTON_LEFT
#define PLATFORM_BUTTON_RIGHT     PSP_BUTTON_RIGHT
#define PLATFORM_BUTTON_X         PSP_BUTTON_SQUARE
#define PLATFORM_BUTTON_Y         PSP_BUTTON_TRIANGLE
#define PLATFORM_BUTTON_A         PSP_BUTTON_CIRCLE
#define PLATFORM_BUTTON_B         PSP_BUTTON_CROSS
#define PLATFORM_SHOULDER_LEFT    PSP_BUTTON_LTRIGGER
#define PLATFORM_SHOULLER_RIGHT   PSP_BUTTON_RTRIGGER
#define PLATFORM_BUTTON_MENU      PSP_BUTTON_START
#define PLATFORM_BUTTON_SELECT    PSP_BUTTON_SELECT
#define PLATFORM_BUTTON_VOLUP     GP2X_BUTTON_VOLUP
#define PLATFORM_BUTTON_VOLDOWN   GP2X_BUTTON_VOLDOWN

//DusteD: Might not work
#define MAJOR_VERSION  1
#define MINOR_VERSION  0
PSP_MODULE_INFO("Wizznic", 0, MAJOR_VERSION, MINOR_VERSION);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(-256);
#define printf pspDebugScreenPrintf

int SetupCallbacks(void);
int exit_callback(int arg1, int arg2, void *common);
int CallbackThread(SceSize args, void *argp);

#define PLATFORM_NEEDS_EXIT
void platformExit();

#endif // PSPSPEC_H

