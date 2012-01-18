#ifndef SWSCALE_H_INCLUDED
#define SWSCALE_H_INCLUDED

#include <SDL/SDL.h>

SDL_Surface* swScaleInit( int sdlVideoModeFlags, int doScale );
void swScale( SDL_Surface* screen, int doScale );

#endif // SWSCALE_H_INCLUDED
