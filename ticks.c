#include "time.h"
#include "text.h"

#include "defs.h"

static int lastTick=0; //SDL clock
static int ticks=0; //ticks since last frame

static int frames=0;
void frameStart()
{
  ticks = SDL_GetTicks() - lastTick;
  lastTick = SDL_GetTicks();
  frames++;
}

static int fpsSecondCounter=0;
static int fps=0;
static char fpsStr[16] = { '0','0','\0' };

inline int getTicks()
{
  return(ticks);
}

inline int getTimeSinceFrameStart()
{
  return( SDL_GetTicks() - lastTick );
}


void drawFPS(SDL_Surface* scr)
{
  fps++;
  fpsSecondCounter+=getTicks();
  if(fpsSecondCounter > 999)
  {
    sprintf(fpsStr, "%i Fps", fps);
    fps=0;
    fpsSecondCounter=0;
  }

  txtWrite(scr,FONTSMALL, fpsStr, HSCREENW-160,HSCREENH-120);
}
