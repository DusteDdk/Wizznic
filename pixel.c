#include "pixel.h"
#include "math.h"
#include "defs.h"

inline void plotPixel(SDL_Surface* img, int x, int y, uint16_t col)
{
  //Bail if invalid position
  if(x < 0 || x > (SCREENW-1) || y < 0 || y > (SCREENH-1) )
  {
    return;
  }
  *(uint16_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;
}

inline void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col)
{
  *(uint16_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;
}

//Well, it also cranks up the intensity to fit 24 bpp.
void readPixel(SDL_Surface* img, int x, int y, int* r, int* g, int* b)
{
  uint16_t col = freadPixel(img,x,y);

  *r = ((col & img->format->Rmask) >> img->format->Rshift)*8;
  *g = ((col & img->format->Gmask) >> img->format->Gshift)*4;
  *b = ((col & img->format->Bmask) >> img->format->Bshift)*8;


};

inline uint16_t freadPixel(SDL_Surface* img, int x, int y)
{
 return( *(uint16_t*)( (char*)(img->pixels)+img->pitch*y+img->format->BytesPerPixel*x ) );
}

