/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2012 Jimmy Christensen <dusted@dusted.dk>             *
 * Wizznic is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * Wizznic is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#include "dumplevelimages.h"
#include "levels.h"
#include "board.h"
#include "draw.h"
#include "cursor.h"
#include "player.h"
#include "pack.h"

void dumplevelimages(SDL_Surface* screen, const char* packName, int dumpStartImage)
{
  //Loop through levels
  int l;
  playField pf;
  cursorType cur;
  char buf[128];
  SDL_Surface* si=0;
  //Set the pack.
  sprintf(buf, "packs/%s",packName);
  packSetByPath( buf );

  for(l=0; l < getNumLevels(); l++)
  {
    printf("Dumping level: %s\n", levelInfo(l)->file);

    initCursor(&cur);

    pf.levelInfo = mkLevelInfo( levelInfo(l)->file );

    if(!loadField(&pf, levelInfo(l)->file ))
    {
      printf("Error: Couldn't init board.\n");
      return;
    }

    if(!initDraw(pf.levelInfo,screen))
    {
      printf("Dump: Error: Couldn't init graphics.\n");
      return;
    }

    //Switch off cursor graphics
    drawDisableCursor(1);

    //Show the "start image"
    si=0;
    if( pf.levelInfo->startImg && dumpStartImage )
    {
      si = loadImg( packGetFile("themes/", pf.levelInfo->startImg) );
    }

    if(si)
    {
      printf("Found image: '%s'\n",packGetFile("themes/", pf.levelInfo->startImg) );
      SDL_BlitSurface( si, 0, screen, 0 );
      SDL_FreeSurface(si);
    } else {
      //Draw the image
      draw(&cur,&pf, screen);
    }

    SDL_Flip(screen);

    //Save image
    sprintf(buf, "%s.tga", levelInfo(l)->file);
    tgaSave(screen, buf);


    cleanUpDraw();
    freeField(&pf);
  }
}

void thumblevelimage(SDL_Surface* screen,const char* lvlfile, const char* outimg)
{
  playField pf;
  cursorType cur;

  packSet(0);

  printf("Thumbnailer.\nDoing: %s >> %s\n",lvlfile,outimg);
  initCursor(&cur);

  if(!loadField(&pf, lvlfile ))
  {
    printf("Error: Couldn't init board.\n");
    return;
  }

  pf.levelInfo = mkLevelInfo( lvlfile );

  if(!initDraw(pf.levelInfo,screen))
  {
    printf("Error: Couldn't init graphics.\n");
    return;
  }

  //Switch off cursor graphics
  drawDisableCursor();

  //Draw the image
  draw(&cur,&pf, screen);

  //Save image
  tgaSave(screen, outimg);

  SDL_Flip(screen);

 // cleanUpDraw();
  freeField(&pf);

}

void tgaSave(SDL_Surface* screen, const char* fileName)
{
  SDL_LockSurface(screen);

  FILE *f = fopen(fileName, "w");

  unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
  unsigned char header[6] = {screen->w%256,screen->w/256,screen->h%256,screen->h/256,24,( 0  | 32)};
  fwrite(TGAheader, sizeof(unsigned char), 12, f);
  fwrite(header, sizeof(unsigned char), 6, f);

  int numBytes = screen->h*screen->w*3;
  unsigned char* data = malloc(sizeof(unsigned char)*numBytes);

  printf("Surf info:\n  Pitch: %i\n  BytesPrPixel: %i\n  BitsPrPixel: %i\n",
  screen->pitch, screen->format->BytesPerPixel, screen->format->BitsPerPixel);

  printf("Rmask: %i\nGmask %i\nBmask %i\nRshift: %i\nGshift %i\nBshift: %i\n",
  screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,
  screen->format->Rshift,screen->format->Gshift,screen->format->Bshift);

  int i;
  uint16_t t;
  //Read pixels as one 16 bit number, then extract rgb values, normalize brightness, (green is too high) and swap red/blue, then save to data
  for(i=0; i<screen->w*screen->h; i++)
  {
    t = *((uint16_t*)screen->pixels+i);

    data[i*3] =((t & screen->format->Bmask) >> screen->format->Bshift) *8;  //blue
    data[i*3+1]= (((t & screen->format->Gmask) >> screen->format->Gshift) ) *4; //green
    data[i*3+2]  = ((t & screen->format->Rmask) >> screen->format->Rshift) *8; //Red color

  }

  //Write data
  fwrite(data, sizeof(unsigned char), numBytes, f);

  //Close file
  fclose(f);

  SDL_UnlockSurface(screen);

  printf("Saving: %s\n",fileName);
}
