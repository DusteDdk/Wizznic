#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED
#include <SDL/SDL.h>
#include "ticks.h"

#define NUMFONTS 4

#define GAMEFONTMEDIUM 3
#define GAMEFONTSMALL 2
#define FONTMEDIUM 1
#define FONTSMALL 0


void txtInit(); //load menu charactersets
void txtLoadGameCharSet(const char* font);
void txtFreeGameCharSet();
void txtWrite( SDL_Surface* scr,int font, const char* txt, int x, int y);
void txtWriteCenter( SDL_Surface* scr,int fontNum, const char* txt, int x, int y);
void txtWave( SDL_Surface* scr, int fontNum, const char* txt, int x, int y, float* rot);
int* getCharSize(int font);

#endif // TEXT_H_INCLUDED
