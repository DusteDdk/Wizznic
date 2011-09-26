#include <math.h>

#include "text.h"
#include "sprite.h"
#include "pack.h"

#ifndef DATADIR
  #define DATADIR ""
#endif

static SDL_Surface* txtSurf[NUMFONTS];
static spriteType* txtSprites[NUMFONTS][91];
static int txtSize[NUMFONTS][2];

//Small is 9x12
//Large is 18x24

void loadFont(const char* imgFileName, int fontNum,int w, int h )
{
  txtSize[fontNum][0] = w;
  txtSize[fontNum][1] = h;

  txtSurf[fontNum] = loadImg( imgFileName );
  if(txtSurf[fontNum]!=NULL)
  {
    int i;
    int xpos=0;
    int ypos=0;
    int linlen=0;
    for(i=0; i < 91; i++)
    {
      txtSprites[fontNum][i] = cutSprite(txtSurf[fontNum],xpos,ypos,txtSize[fontNum][0],txtSize[fontNum][1]);
      xpos+=txtSize[fontNum][0];
      linlen++;
      if(linlen==16)
      {
        linlen=0;
        ypos += txtSize[fontNum][1];
        xpos=0;
      }
    }
  } else {
    printf("fontLoad(); Can't load '%s'\n", imgFileName);
  }
}

void txtLoadGameCharSet(const char* font)
{
  char* buf = malloc( sizeof(char)*128 );
  sprintf( buf, "%s0.png", font );
  loadFont( packGetFile( "themes/chars/",buf),GAMEFONTSMALL,9,12);
  sprintf( buf, "%s1.png", font );
  loadFont( packGetFile( "themes/chars/",buf),GAMEFONTMEDIUM,18,24);
  free( buf );
}

void txtFreeGameCharSet()
{
  int i;
  for(i=0; i < 91;i++)
  {
    free( txtSprites[GAMEFONTSMALL][i] );
    free( txtSprites[GAMEFONTMEDIUM][i] );
  }

  SDL_FreeSurface(txtSurf[GAMEFONTSMALL]);
  SDL_FreeSurface(txtSurf[GAMEFONTMEDIUM]);
}

void txtInit()
{
  loadFont( DATADIR"data/menu/charmap0.png", FONTSMALL,9,12);
  loadFont( DATADIR"data/menu/charmap1.png", FONTMEDIUM,18,24);
}

void txtWrite( SDL_Surface* scr,int fontNum, const char* txt, int x, int y)
{
  int px=x;
  int py=y;
  int pos=0;
  char c;
  c = txt[pos];

  while( c != '\0')
  {
    pos++;
    if(c=='\n')
    {
      py+=txtSize[fontNum][1];
      px=x-txtSize[fontNum][0];
    }
    else if(c!=' ' && c>-1 && c<123) //Don't draw spaces or unicode chars. (like danish)
    {
      drawSprite(scr, txtSprites[fontNum][(int)c-32],px,py);
    }
    c = txt[pos];
    px+=txtSize[fontNum][0];
  }
}

void txtWriteCenter( SDL_Surface* scr,int fontNum, const char* txt, int x, int y)
{
  int len = txtSize[fontNum][0]*strlen(txt);

  x -= len/2;
  txtWrite(scr,fontNum,txt,x,y);
}

void txtWave( SDL_Surface* scr, int fontNum, const char* txt, int x, int y, float* rot)
{
  //dec rot
  *rot -= (float)getTicks()/200.0;
  //For each char:
  float amp=6;
  int px=x;
  int py=y;
  int pos=0;
  int len=strlen(txt);
  float chInc = 6.28318531/len;
  px = x - (txtSize[fontNum][0]*len)/2;
  char c;
  c = txt[pos];

  while( c != '\0')
  {
    pos++;
    if(c=='\n')
    {
      py+=txtSize[fontNum][1];
      px=x-txtSize[fontNum][0];
    }
    else if(c!=' ')
    {
      drawSprite(scr, txtSprites[fontNum][(int)c-32],px,py+(sin(*rot+(pos*chInc))*amp));
    }
    c = txt[pos];
    px+=txtSize[fontNum][0];
  }
}

int* getCharSize(int font)
{
  return(txtSize[font]);
}
