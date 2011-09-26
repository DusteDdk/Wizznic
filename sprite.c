#include "sprite.h"

SDL_Surface* loadImg( const char* fileName )
{
  //Load the surface
  SDL_Surface* unoptimized = NULL;
  SDL_Surface* optimized = NULL;

  unoptimized = IMG_Load( fileName ); //Cast that makes no sense?

  if(unoptimized!=NULL)
  {
    //Create optimized.
    optimized = SDL_DisplayFormat( unoptimized );

    //Destroy old img
    SDL_FreeSurface( unoptimized );

    if(optimized!=NULL)
    {
      //Set colorkey for cheap transparency
      SDL_SetColorKey( optimized, SDL_SRCCOLORKEY, SDL_MapRGB( optimized->format, 0, 0xFF, 0xFF ) );
    }
  }

  return(optimized);
}

spriteType* cutSprite(SDL_Surface* img, int x,int y, int w, int h)
{
  //Did we get a valid surface?
  if(!img) return(NULL);

  //Allocate memory for the sprite
  spriteType* tspr = malloc( sizeof(spriteType) );
  if(tspr != NULL)
  {
    tspr->img = img;
    tspr->clip.x = x;
    tspr->clip.y = y;
    tspr->clip.w = w;
    tspr->clip.h = h;
  } else {
    printf("Error, couldn't malloc %i bytes.\n",sizeof(spriteType));
  }

  return(tspr);
}

void drawSprite(SDL_Surface* scr, spriteType* spr, int x, int y)
{
  if(!spr) return;
  SDL_Rect pos;
  pos.x = x;
  pos.y = y;
  SDL_BlitSurface( spr->img, &spr->clip, scr, &pos );
}
