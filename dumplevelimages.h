#ifndef DUMPLEVELIMAGES_H_INCLUDED
#define DUMPLEVELIMAGES_H_INCLUDED
#include <SDL/SDL.h>
void dumplevelimages(SDL_Surface* screen, const char* packName, int dumpStartImage);
void tgaSave(SDL_Surface* screen, const char* fileName);
void thumblevelimage(SDL_Surface* screen,const char* lvlfile, const char* outimg);

#endif // DUMPLEVELIMAGES_H_INCLUDED
