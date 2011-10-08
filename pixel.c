#include "pixel.h"
#include "math.h"
#include "defs.h"

#include <GL/gl.h>



void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
 int bpp = surface->format->BytesPerPixel;
 // Here p is the address to the pixel we want to set
 Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

 switch(bpp)
 {
  case 1:
   *p = pixel;
   break;
  case 2:
   *(Uint16 *)p = pixel;
   break;
  case 3:
   if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
   {
    p[0] = (pixel >> 16) & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = pixel & 0xff;
   } else {
    p[0] = pixel & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = (pixel >> 16) & 0xff;
   }
   break;
  case 4:
   *(Uint32 *)p = pixel;
   break;
 }
}



//inline void plotPixel(SDL_Surface* img, int x, int y, uint16_t col)
inline void plotPixel(SDL_Surface* img, int x, int y, uint32_t col)
{
  //Bail if invalid position
  if(x < 0 || x > (SCREENW-1) || y < 0 || y > (SCREENH-1) )
  {
    return;
  }

  putpixel(img,x,y,col);

//  *(uint16_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;*/
}

inline void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col)
{
  *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+2*x ) = col;
}


inline uint32_t freadPixel(SDL_Surface* img, int x, int y)
{
 return( *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+img->format->BytesPerPixel*x ) );
}

