#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

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

#include "defs.h"
#include "cursor.h"
#include "list/list.h"
#include "sound.h"
#include "levels.h"
#include "teleport.h"

#define NOBRICKSLEFT -1
#define UNSOLVABLE -2
#define LIFELOST -3

struct brick_t
{
  int type;
  int dx, dy; //destionation positions, for when moving
  int sx, sy; //source destionation, for clearing block when they moved
  int dmx; //"Delayed movement" for if moving a brick when it's between bricks.
  int moveXspeed, moveYspeed;
  int pxx, pxy; //Position in pixels
  int curLock; //Cursor locked to brick

  int dir; //Used to switch direction on movers, otherwise used to indicate that brick is just added to removelist
  int tl; //dirchange time left, used for kill timeout too

  int checked; //Have this brick been checked in this loop ? (for bricks that moved)
  int_fast16_t edges; //Only used by wall bricks, bit decides edge

  int isActive; //Only used by switches, if 1, the switch have activated it's target (if 0, the target is in the deactivated list).
  struct brick_t* target; //Only used by switched, target brick, is set upon init.
};
typedef struct brick_t brickType;

struct playField_t
{
  levelInfo_t* levelInfo;

  brickType* board[FIELDSIZE][FIELDSIZE]; //Matrix of pointers, initialized to 0 by loadmap

  //lol
  brickType* blocker; //Universial, invisible, quite magic blocker, for reserving space when bricks are travelling.
  brickType* blockerDst;

  list_t* movingList; //Start of the linked list of moving bricks

  list_t* deactivated;  //Bricks that are deactivated by a switch.

  int brickTypes[BRICKSEND]; //Number of bricks of each type currently on board

  list_t* removeList; //Start of the linked list of bricks that's going to die, tl counts down their lifespan
  int_fast8_t newWalls; //Used to indicate that walls have changed on this board.

};

typedef struct playField_t playField;

void boardSetWalls(playField* pf);
int loadField(playField* pf, const char* file); //Henter et spillefelt med filnavnet, retunerer 0 ved fejl.
void freeField(playField* pf); //Frees allocated memory
void simField(playField* pf, cursorType* cur); //Does logic on the field (gravity/moving bricks)
int doRules(playField* pf); //Does gameRules, returns number of bricks destroyed, returns -1 when no more bricks left.
int moveBrick(playField* pf, int x, int y, int dirx,int diry, int block, int speed); //Move a brick within the field (if possible) in either 0:Left, 1: Right, returns 1 on success

void telePortBrick(playField* pf,telePort_t* t, cursorType* cur);

int isBrickFalling(playField* pf, brickType* b);
int isBrick(brickType* b);
int isSwitch(brickType* b);

int isWall(playField* pf, int x, int y);
int isMover(brickType* b);
int isOneWay(brickType* b);
int curMoveBrick(playField *pf, brickType *b, int dir);

brickType* findMoving(playField* pf, int x, int y);
brickType* brickUnderCursor(playField* pf, int x, int y);

int boardDestroyNextBrick(playField* pf);

int saveLevel(const char* fileName, playField* pf);
int isLevelCompletable(const char* fileName);
void setLevelCompletable(const char* fileName, int_fast8_t completable);

#endif // BOARD_H_INCLUDED
