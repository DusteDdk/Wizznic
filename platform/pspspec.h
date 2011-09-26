// Sony PSP specific values and combatibility functions

#ifndef PSPSPEC_H
#define PSPSPEC_H

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

int SetupCallbacks(void);
int exit_callback(int arg1, int arg2, void *common);
int CallbackThread(SceSize args, void *argp);

#endif // PSPSPEC_H

