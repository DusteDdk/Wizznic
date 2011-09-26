#ifndef STARS_H_INCLUDED
#define STARS_H_INCLUDED

#include <SDL/SDL.h>
#include "list.h"

struct star_s {
  int x, y, sx,sy;
  uint32_t color;
  int life;
  int fade;
};
typedef struct star_s star_t;

struct rocket_s {
  int x, y, sx, sy;
  int life;
  listItem* p;
};
typedef struct rocket_s rocket_t;

void initStars(SDL_Surface* screen);
void starField(SDL_Surface* screen, int move);
void fireWorks(SDL_Surface* screen);

#endif // STARS_H_INCLUDED
