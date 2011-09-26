#ifndef LEVELSELECTOR_H_INCLUDED
#define LEVELSELECTOR_H_INCLUDED

#include "sprite.h"
#include "levels.h"

void levelSelector(SDL_Surface* screen, int l, int stats); //Show the level selector
void drawPreviewImg(SDL_Surface* screen, SDL_Surface* img, int stats);
void resetLevelSelector(); //used for when changing pack

#endif // LEVELSELECTOR_H_INCLUDED
