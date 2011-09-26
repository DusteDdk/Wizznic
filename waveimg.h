#ifndef WAVEIMG
#define WAVEIMG

#include <SDL/SDL.h>
struct wavingImage_s
{
  SDL_Surface *screen, *img;
  int x,y,rotations,amount,speed;
  float privRotAmount;
};
typedef struct wavingImage_s wavingImage_t;
//void waveImg(SDL_Surface* screen, SDL_Surface* img, int x, int y,int rots, int amount, int speed);
void waveImg(wavingImage_t* wi);

void setWaving(wavingImage_t* wi, SDL_Surface* screen, SDL_Surface* img, int x, int y, int rots, int amount, int speed);

#endif
