#include "pointer.h"
#include "ticks.h"
#include "defs.h"
#include "sprite.h"
#include "pixel.h"

static inpPointerState_t inpPointer;
static SDL_Surface* ptrBackImg;
static SDL_Rect ptrBackRect;

inline inpPointerState_t* getInpPointerState()
{
  return(&inpPointer);
}

void initPointer(SDL_Surface* screen)
{
  //Load the "escape" overlay image
  ptrBackImg = loadImg(DATADIR"/data/ptr-back.png");
  if( !ptrBackImg )
    printf("Couldn't open %s\n",DATADIR"/data/ptr-back.png");

  ptrBackRect.x=0;
  ptrBackRect.y=0;
  ptrBackRect.w=ptrBackImg->w;
  ptrBackRect.h=ptrBackImg->h;

  memset( &inpPointer, 0, sizeof(inpPointerState_t) );
  inpPointer.timeSinceMoved=POINTER_SHOW_TIMEOUT;

  //The color white
  inpPointer.colWhite = SDL_MapRGBA( screen->format, 255,255,255,255 );

  inpPointer.escEnable=1;

}

inline void drawPointer(SDL_Surface* screen)
{
  if( inpPointer.timeSinceMoved < POINTER_SHOW_TIMEOUT  )
  {
    //Update pointer-holddown time
    if(inpPointer.isDown)
      inpPointer.downTime += getTicks();

    SDL_BlitSurface( ptrBackImg, NULL, screen,NULL);

    plotPixel(screen, inpPointer.vpX, inpPointer.vpY-2, inpPointer.colWhite );
    plotPixel(screen, inpPointer.vpX, inpPointer.vpY+2, inpPointer.colWhite );

    plotPixel(screen, inpPointer.vpX-2, inpPointer.vpY, inpPointer.colWhite );
    plotPixel(screen, inpPointer.vpX+2, inpPointer.vpY, inpPointer.colWhite );

  }
}

int_fast8_t isPointerClicked()
{
  if( inpPointer.isDown && inpPointer.downTime==0 )
  {
    return(1);
  }
  return(0);
}

int_fast8_t isBoxClicked( SDL_Rect* r )
{
  if( isPointerClicked() )
  {
    if( r->x < inpPointer.vpX && r->w > inpPointer.vpX && r->y < inpPointer.vpY && r->h > inpPointer.vpY )
    {
      inpPointer.isDown=0;
      inpPointer.downTime=1;
      inpPointer.hitABox=1;
      return(1);
    }
  }
  return(0);
}

int_fast8_t isPointerEscapeClicked()
{
  if( inpPointer.escEnable && isBoxClicked( &ptrBackRect ) )
  {
    inpPointer.hitABox=0;
    return(1);
  }
  return(0);
}

int_fast8_t isAnyBoxHit()
{
  return(inpPointer.hitABox);
}
