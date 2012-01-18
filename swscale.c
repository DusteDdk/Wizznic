#include "swscale.h"
#include "settings.h"
#include "defs.h"
#include "pixel.h"

static SDL_Surface* scale;

SDL_Surface* swScaleInit( int sdlVideoModeFlags, int doScale )
{
  scale = SDL_SetVideoMode(SCREENW*doScale,SCREENH*doScale,16, sdlVideoModeFlags);
  SDL_Surface* screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320,240,16, scale->format->Rmask,scale->format->Gmask,scale->format->Bmask,0xff000000);

  //Set scaling
  setting()->scaleFactor= (float)scale->h/240.0;

  return( screen );
}

void swScale( SDL_Surface* screen, int doScale )
{
    if(doScale==2)
    {
      int x,xx,y,yy;
      for(y=0; y< SCREENH; y++)
      {
        for(x=0; x < SCREENW; x++)
        {
          uint16_t c = freadPixel(screen,x,y);/*SDL_MapRGB(scale->format,r,g,b);*/
          xx=x*2;
          yy=y*2;
          plotPixelu(scale, xx,yy, c);
          plotPixelu(scale, xx+1,yy, c);
          plotPixelu(scale, xx,yy+1, c);
          plotPixelu(scale, xx+1,yy+1, c);
        }
      }
    } else if(doScale>2)
    {
      int x,y;
      SDL_Rect r;
      for(y=0; y< SCREENH; y++)
      {
        for(x=0; x < SCREENW; x++)
        {
          r.x=x*doScale;
          r.y=y*doScale;
          r.w=doScale;
          r.h=doScale;
          SDL_FillRect(scale, &r, freadPixel(screen,x,y));
        }
      }
    }

    if( doScale > -1 )
    {
      SDL_Flip(scale);
    }
}
