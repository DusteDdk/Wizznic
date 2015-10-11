/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2015 Jimmy Christensen <dusted@dusted.dk>             *
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
 * along with Wizznic.  If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

#include "dumplevelimages.h"
#include "../levels.h"
#include "../board.h"
#include "../draw.h"
#include "../cursor.h"
#include "../player.h"
#include "../pack.h"

void dumplevelimages(SDL_Surface* screen, const char* packName, int dumpStartImage)
{
  //Loop through levels
  int l;
  playField pf;
  cursorType cur;
  char buf[128];
  SDL_Surface* si=0;
  //Set the pack.
  sprintf(buf, DATADIR"packs/%s",packName);
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

    tgaData_t* tga = tgaData(screen);
    tgaSave(tga, buf);
    tgaFree(tga);


    cleanUpDraw();
    freeField(&pf);
  }
}



void dumpOneLevelFile(SDL_Surface* screen, const char* fileName)
{
  playField pf;
  cursorType cur;
  char buf[128];
  //Set the pack.
  packSetByPath( DATADIR"packs/000_wizznic" );


    printf("Dumping level: %s to %s.tga\n", fileName, fileName);

    initCursor(&cur);

    pf.levelInfo = mkLevelInfo( fileName );

    if(!loadField(&pf, fileName ))
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


    //Draw the image
    draw(&cur,&pf, screen);


    SDL_Flip(screen);

    //Save image
    sprintf(buf, "%s.tga", fileName);

    tgaData_t* tga = tgaData(screen);
    tgaSave(tga, buf);
    tgaFree(tga);

    cleanUpDraw();
    freeField(&pf);
    printf("Done.\n");
}


tgaData_t* tgaData(SDL_Surface* screen)
{
  SDL_LockSurface(screen);
  tgaData_t* tga = malloc(sizeof(tgaData_t));

  tga->len = (screen->h*screen->w*3)+(12+6);
  tga->data = malloc(tga->len*60); //try allocating for 60 fps more, if that's not possible, we bail.

  if(tga->data)
  {
    free(tga->data);
  } else {
    free(tga);
    printf("tgaData(); Not enough memory to take screenshot.\n");
    return(NULL);
  }

  tga->data = malloc(tga->len);


  //Tga header
  tga->data[0] = 0;
  tga->data[1] = 0;
  tga->data[2] = 2;
  tga->data[3] = 0;
  tga->data[4] = 0;
  tga->data[5] = 0;
  tga->data[6] = 0;
  tga->data[7] = 0;
  tga->data[8] = 0;
  tga->data[9] = 0;
  tga->data[10] = 0;
  tga->data[11] = 0;

  //File info
  tga->data[12] = screen->w%256;
  tga->data[13] = screen->w/256;
  tga->data[14] = screen->h%256;
  tga->data[15] = screen->h/256;
  tga->data[16] = 24;
  tga->data[17] = ( 0  | 32);


#ifdef DEBUG
  printf("Surf info:\n  Pitch: %i\n  BytesPrPixel: %i\n  BitsPrPixel: %i\n",
  screen->pitch, screen->format->BytesPerPixel, screen->format->BitsPerPixel);

  printf("Rmask: %i\nGmask %i\nBmask %i\nRshift: %i\nGshift %i\nBshift: %i\n",
  screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,
  screen->format->Rshift,screen->format->Gshift,screen->format->Bshift);
 #endif

  int i;
  uint16_t t;
  //Read pixels as one 16 bit number, then extract rgb values, normalize brightness, (green is too high) and swap red/blue, then save to data
  unsigned char* imgData=&tga->data[18];
  for(i=0; i<screen->w*screen->h; i++)
  {
    if(screen->format->BytesPerPixel==2)
    {
      t = *((uint16_t*)screen->pixels+i);

      imgData[i*3] =((t & screen->format->Bmask) >> screen->format->Bshift) *8;  //blue
      imgData[i*3+1]= (((t & screen->format->Gmask) >> screen->format->Gshift) ) *4; //green
      imgData[i*3+2]  = ((t & screen->format->Rmask) >> screen->format->Rshift) *8; //Red color
    } else {
      imgData[i*3]=(unsigned char)((unsigned char*)screen->pixels)[i*3];
      imgData[i*3+1]=(unsigned char)((unsigned char*)screen->pixels)[i*3+1];
      imgData[i*3+2]=(unsigned char)((unsigned char*)screen->pixels)[i*3+2];
    }

  }
  SDL_UnlockSurface(screen);
  return(tga);
}

void tgaSave(tgaData_t* tga, const char* fileName)
{

  FILE *f = fopen(fileName, "w");

#ifdef DEBUG
  printf("Saving: %s\n",fileName);
#endif
  //Write data
  fwrite(tga->data, sizeof(unsigned char), tga->len, f);

  //Close file
  fclose(f);
}

void tgaFree(tgaData_t* tga)
{
  free(tga->data);
  free(tga);
}

static SDL_Surface* scr;
void screenShotSetCaptureScreen(SDL_Surface* screen)
{
  scr=screen;
}

void screenShot()
{
  tgaData_t* tga = tgaData(scr);
  char scrName[2048];
  int num=0;

  //Find next nonexisting match of Wizznic_%02i.tga"
  do
  {
    sprintf(scrName, "Wizznic_%02i.tga", num);
    if( isFile(scrName) )
    {
      scrName[0]=0;
    }
    num++;
  } while (!scrName[0]);

  printf("Saving %s\n",scrName);
  tgaSave(tga, scrName);
  tgaFree(tga);
}

