#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct {
  SDL_Rect clip; //Position in the surface
  SDL_Surface* img; //The surface where this sprite resides, shared between sprites.
} spriteType;

SDL_Surface* loadImg( const char* fileName ); //Ret 0 fail
spriteType* cutSprite(SDL_Surface* img, int x,int y, int w, int h); //Ret 0 fail
void drawSprite(SDL_Surface* scr, spriteType* spr, int x, int y);


#endif // SPRITE_H_INCLUDED
