#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include <SDL/SDL.h>
#include "defs.h"
#include "board.h"
#include "cursor.h"
#include "sprite.h"
#include "list.h"
#include "particles.h"
#include "levels.h"
#include "teleport.h"

#define TELEPATHNUMCOL 32
struct boardGraphics_t
{
  SDL_Surface* boardImg;
  SDL_Surface* tileImg;
  SDL_Surface* curImg;
  SDL_Surface* wallImg; //If theme uses extra wall tiles.
  SDL_Surface* explImg[BRICKSEND]; //Brick destroy animations, either one pr tile, or all defaults to 0.
  SDL_Surface* countDownImg;
  spriteType* curSpr[2];
  spriteType* tiles[NUMTILES];
  spriteType* expl[BRICKSEND][16]; //16 frame animation for each type of brick
  spriteType* walls[16]; //Extra walls
  spriteType* countDownSpr[4]; //Countdown graphics 3,2,1,start

  //Teleport path animation
  int teleColorIndex;
  uint16_t teleColorTable[TELEPATHNUMCOL];
};

int initDraw(levelInfo_t* li);
void cleanUpDraw();
void draw(cursorType* cur, playField* pf, SDL_Surface* screen);
void drawTelePath( SDL_Surface* screen, telePort_t* tp, int animate );
void drawPath( SDL_Surface* screen, int sx, int sy, int dx, int dy, int animate );
void drawAllTelePaths( SDL_Surface* screen,listItem* tp );
void drawShowCountDown(SDL_Surface* screen, int i);
struct boardGraphics_t* stealGfxPtr();
//For editor
void drawBrick(SDL_Surface* screen, int brickNum, int x, int y);

//For image dumper
#ifndef GP2X
void drawDisableCursor();
#endif

#endif // DRAW_H_INCLUDED
