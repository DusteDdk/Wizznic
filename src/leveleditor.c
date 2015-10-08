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

#include "settings.h"
#include "leveleditor.h"
#include "cursor.h"
#include "board.h"
#include "draw.h"
#include "states.h"
#include "input.h"
#include "text.h"
#include "levels.h"
#include "list/list.h"
#include <stdio.h>

#include "strings.h"
#include "teleport.h"

#include "transition.h"
#include "defs.h"

#define EDITOR_MAIN 0
#define EDITOR_BRICKS_SELECTION 1
#define EDITOR_SAVEBTN_CLICKED 3 /*Has to be 3 */

static playField pf;
static cursorType cur;
static int selBrick=1;
static int changed=0;
static char fileName[64];
static int teleState=0; //Teleport placement iteration
static int teleSrcPos[2];
static int editorState;
SDL_Surface* selBrickBG;
static int allowBrickToBePlaced;

SDL_Surface* saveBtnBG;
spriteType* saveBtnSprite;

static SDL_Rect fieldRect; //Used to detect when the cursor is inside the brick-field.

void editorLoad(const char* fn, SDL_Surface* screen)
{
  //Set filename
  editorFileName(fn);

  //Init cursor
  initCursor(&cur);
  //Read info's for level.
  pf.levelInfo = mkLevelInfo( fn );
  //Load field
  loadField(&pf, fn);

  initDraw(pf.levelInfo, screen);
  SDL_FreeSurface(stealGfxPtr()->boardImg);
  stealGfxPtr()->boardImg = loadImg( DATADIR"data/editbg.png" );

  selBrickBG = loadImg( DATADIR"data/editselbrick.png" );

  saveBtnBG = loadImg( DATADIR"data/edit-save.png" );
  saveBtnSprite = cutSprite( saveBtnBG, 0,0, saveBtnBG->w, saveBtnBG->h );

  changed=0;
  selBrick=BRICKSBEGIN;

  teleState=0;

  fieldRect.x = HSCREENW -  74;
  fieldRect.y = HSCREENH - 114;
  fieldRect.w = HSCREENW + 154;
  fieldRect.h = HSCREENH + 114;
}

void editorCleanUp()
{

  resetBtn(C_UP);
  resetBtn(C_DOWN);
  resetBtn(C_LEFT);
  resetBtn(C_RIGHT);
  resetBtn(C_BTNX);
  resetBtn(C_BTNB);

  //Free memory used for levelInfo
  freeLevelInfo( &pf.levelInfo );
  //Free board and graphics here
  cleanUpDraw();
  changed=0;
  freeField(&pf);

  //Free graphics
  SDL_FreeSurface(selBrickBG);

  SDL_FreeSurface(saveBtnBG);
  free(saveBtnSprite);
}

void editorFileName(const char* fn)
{
  changed=1;
  strcpy(fileName,fn);
}

void editorPickBrickUnderCursor()
{
  if(pf.board[cur.x][cur.y])
  {
    selBrick=pf.board[cur.x][cur.y]->type;
  }
}

int editIsSwitch(int s)
{
  return(s==SWON||s==SWOFF);
}

void editorRemoveBrickUnderCursor()
{
  if(pf.board[cur.x][cur.y])
  {
    //Switch?
    if( editIsSwitch(pf.board[cur.x][cur.y]->type) )
    {
      teleRemoveFromList(pf.levelInfo->switchList,cur.x,cur.y);
    }
    free(pf.board[cur.x][cur.y]);
    pf.board[cur.x][cur.y]=0;
  }

  //teleport?
  teleRemoveFromList(pf.levelInfo->teleList,cur.x,cur.y);



  boardSetWalls(&pf);
  changed=1;
}

void editAddToBoard(int s)
{
  pf.board[cur.x][cur.y]=malloc(sizeof(brickType));
  pf.board[cur.x][cur.y]->type=s;
  pf.board[cur.x][cur.y]->isActive=1;
  pf.board[cur.x][cur.y]->pxx=cur.x*20+boardOffsetX;
  pf.board[cur.x][cur.y]->pxy=cur.y*20+boardOffsetY;
  boardSetWalls(&pf);
}


int runEditor(SDL_Surface* screen)
{
  SDL_Rect selBrickRect;
  getInpPointerState()->escEnable=1;

  if( editorState == EDITOR_MAIN )
  {

    if(getButton(C_BTNMENU) || isPointerEscapeClicked() )
    {
      resetBtn( C_BTNMENU );
      resetMouseBtn();

      changed++; //If it was 0 then it will become 1 (saved) exit. If it was 1 it becomes 2 (not saved).
      if( changed != 2 )
      {
        editorCleanUp();
        startTransition(screen, TRANSITION_TYPE_ROLL_IN, 500 );
        return(STATEMENU);
      }
    }

    //We detect if the "preview" brick on the left is clicked, we do this now so we can reset the click so that it does not hit the board
    selBrickRect.x = HSCREENW-125;
    selBrickRect.y = HSCREENH-85;
    selBrickRect.w = selBrickRect.x+20;
    selBrickRect.h = selBrickRect.y+20;
    //Also, we can only click it if a teleport destination is not being placed.
    if( isBoxClicked(&selBrickRect) && teleState==0 )
    {
      editorState=EDITOR_BRICKS_SELECTION;
      resetMouseBtn();
    }

    //We detect mouse-save input here so it won't hit the board.
    selBrickRect.x = HSCREENW-145;
    selBrickRect.y = HSCREENH+42;
    selBrickRect.w = selBrickRect.x+59;
    selBrickRect.h = selBrickRect.y+24;
    if( isBoxClicked(&selBrickRect) && changed>0)
    {
      changed=EDITOR_SAVEBTN_CLICKED;
      resetMouseBtn();
    }

    //We check if the cursor is in the field (if it is not, brick-placement is blocked so we don't place bricks when clicking outside of the field).
    if( isPointerInBox(&fieldRect) || getInpPointerState()->timeSinceMoved > POINTER_SHOW_TIMEOUT )
    {
      allowBrickToBePlaced=1;
    } else {
      allowBrickToBePlaced=0;
    }

    //Handle movement
    if(getButton(C_UP))
    {
      resetBtn(C_UP);
      moveCursor(&cur, 0,DIRUP, 0);
    }

    if(getButton(C_DOWN))
    {
      resetBtn(C_DOWN);
      moveCursor(&cur, 0,DIRDOWN,0);
    }

    if(getButton(C_LEFT))
    {
      resetBtn(C_LEFT);
      moveCursor(&cur, DIRLEFT,0, 0);
    }

    if(getButton(C_RIGHT))
    {
      resetBtn(C_RIGHT);
      moveCursor(&cur, DIRRIGHT,0, 0);
    }

    //Handle mouse input
    if( getInpPointerState()->timeSinceMoved==0 && !cur.lock )
    {
      setCursor(&cur, getInpPointerState()->curX,getInpPointerState()->curY );
    }

    if(getButton(C_BTNB))
    {
      resetBtn(C_BTNB);
      selBrick++;

      if(selBrick==RESERVED)
        selBrick++;

      if(selBrick>NUMTILES)
        selBrick=1;
    }

    if(getButton(C_BTNA))
    {
      resetBtn(C_BTNA);

      selBrick--;
      if(selBrick==RESERVED)
        selBrick--;

      if(selBrick<1)
        selBrick=NUMTILES;
    }



    //Is place brick button being pressed, and if it is, are we allowed to place the brick?
    if( (getButton(C_BTNX) || getInpPointerState()->isDown ) && selBrick != RESERVED && allowBrickToBePlaced )
    {

      //We remove the brick before placing a new one if it's not a teleport or if it's a switch (not switch-target).
      if( selBrick!=TELESRC && !((editIsSwitch(selBrick)&&teleState==1)  ) )
      {
        editorRemoveBrickUnderCursor();
      }

      if(selBrick==TELESRC || editIsSwitch(selBrick) )
      {
        resetMouseBtn();
        resetBtn(C_BTNX);

        if(teleState==0)
        {
          //Save source pos
          teleSrcPos[0] = cur.x;
          teleSrcPos[1] = cur.y;
          teleState++;
        } else {
          //Add to list
          if(editIsSwitch(selBrick))
          {
            teleAddToList( pf.levelInfo->switchList, teleSrcPos[0], teleSrcPos[1], cur.x, cur.y );
            //printf("Number of members in switchList: %i\n", listSize(pf.levelInfo->switchList) );
            cur.x = teleSrcPos[0];
            cur.y = teleSrcPos[1];
            editAddToBoard(selBrick);
          } else {
            teleAddToList( pf.levelInfo->teleList, teleSrcPos[0], teleSrcPos[1], cur.x, cur.y );
          }
          //Reset state
          teleState=0;
        }
      } else {
        editAddToBoard(selBrick);
      } //Not a teleport

      changed=1;
    }

    if( getButton(C_BTNY) )
    {
      //If we are trying to remove an empty teleport
      if(telePresent(pf.levelInfo->teleList, cur.x, cur.y) && selBrick!=TELESRC)
      {
        resetBtn(C_BTNY);
        selBrick=TELESRC;
      } else {
        if(selBrick!=RESERVED)
        {
          editorPickBrickUnderCursor();
        }
        editorRemoveBrickUnderCursor();
      }
    }

    if( getInpPointerState()->isDown && selBrick==RESERVED )
    {
      editorRemoveBrickUnderCursor();
    }

    if(getButton(C_BTNSELECT) || changed==EDITOR_SAVEBTN_CLICKED)
    {
      resetBtn(C_BTNSELECT);

      pf.levelInfo->completable=0;
      if( saveLevel(fileName, &pf) )
      {
        //Refresh the list of userLevels.
        addUserLevel(fileName);
        changed=0;
      }


    }

  } //Editor in main state, don't ignore input


  draw(&cur, &pf, screen);


  if(changed==2)
  {
    txtWriteCenter(screen, FONTMEDIUM, STR_EDIT_NOT_SAVED_WARNING, HSCREENW,HSCREENH-20);
    txtWriteCenter(screen, FONTSMALL, STR_EDIT_PRESS_EXIT_TO_EXIT, HSCREENW,HSCREENH);
    txtWriteCenter(screen, FONTSMALL, STR_EDIT_PRESS_SAVE_TO_SAVE, HSCREENW,HSCREENH+10);
  }


  txtWriteCenter(screen, FONTSMALL,STR_EDIT_STATUS, HSCREENW-115,HSCREENH+80);
  txtWriteCenter(screen, FONTSMALL, (changed)?STR_EDIT_UNSAVED:STR_EDIT_SAVED, HSCREENW-115,HSCREENH+89);

  txtWriteCenter(screen, FONTSMALL,fileName, HSCREENW,HSCREENH+110);

  txtWriteCenter(screen, FONTSMALL,STR_EDIT_CONTROLS, HSCREENW,HSCREENH-120);


  //Write which keys are used to cycle selected brick.
  txtWriteCenter(screen, FONTSMALL,STR_EDIT_PREVBRICK_KEY,HSCREENW-142,HSCREENH-80);
  txtWriteCenter(screen, FONTSMALL,STR_EDIT_NEXTBRICK_KEY,HSCREENW-88,HSCREENH-80);


  //Draw the currently selected brick.
  drawBrick(screen, selBrick,HSCREENW-125,HSCREENH-85);

  //Write brick name.
  txtWriteCenter(screen, FONTSMALL, str_brick_names[selBrick], HSCREENW-116,HSCREENH-56 );

  //Tell if we're placing teleport source or destination
  if(selBrick==TELESRC && teleState==0)
  {
    txtWriteCenter(screen, FONTSMALL, "(From)", HSCREENW-115,HSCREENH-41);
  } else if(teleState)
  {
    if(selBrick==TELESRC)
    {
      txtWriteCenter(screen, FONTSMALL, "(To)", HSCREENW-115,HSCREENH-41);
    } else {
      txtWriteCenter(screen, FONTSMALL, "(Target)", HSCREENW-115,HSCREENH-41);
    }
    drawPath(screen, teleSrcPos[0], teleSrcPos[1], cur.x, cur.y, 1);
  }

  //Draw all the telepaths.
  drawAllTelePaths(screen, pf.levelInfo->teleList);


  //Draw switchpath we hover above
  listItem* t = &pf.levelInfo->switchList->begin;
  while( LISTFWD(pf.levelInfo->switchList, t) )
  {
    telePort_t* tp=(telePort_t*)t->data;
    if(cur.x == tp->sx && cur.y == tp->sy)
    {
      drawTelePath( screen, tp, 1 );
    }
  }
  //Draw all switchpaths
  drawAllTelePaths(screen, pf.levelInfo->switchList);

  if( (getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT) && (changed > 0) )
  {
    drawSprite(screen, saveBtnSprite, HSCREENW-145, HSCREENH+42 );
  }

  //Draw brick-selection
  if( editorState == EDITOR_BRICKS_SELECTION )
  {
    SDL_BlitSurface(selBrickBG , NULL, screen, &(setting()->bgPos) );

    //Draw bricks in a 6*4 grid
    int px,py,bnum=BRICKSBEGIN;
    static int brickSelOfX = HSCREENW - 78 + 8;
    static int brickSelOfY = HSCREENH - 54 + 8;
    for(py=0;py < 4; py++)
    {
      for(px=0; px < 6; px++)
      {
        if( bnum > NUMTILES )
          break;
        selBrickRect.x = brickSelOfX+(24*px);
        selBrickRect.y = brickSelOfY+(24*py);
        selBrickRect.w = selBrickRect.x+20;
        selBrickRect.h = selBrickRect.y+20;


        //We set bricks on mouseover, this way we get the description too (maybe punch a hole in the dots where the text is?)
        if( isPointerInBox(&selBrickRect) )
        {
          selBrick=bnum;
        }

        //We continue back to the main editor
        if( isPointerClicked() )
        {
          resetMouseBtn();
          editorState=EDITOR_MAIN;
        }

        drawBrick(screen, bnum, selBrickRect.x, selBrickRect.y );
        bnum++;
      }
    }
  }

  return(STATEEDIT);
}


