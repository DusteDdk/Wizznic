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

#include "settings.h"
#include "leveleditor.h"
#include "cursor.h"
#include "board.h"
#include "draw.h"
#include "states.h"
#include "input.h"
#include "text.h"
#include "levels.h"
#include "list.h"
#include <stdio.h>

#include "strings.h"
#include "teleport.h"

#include "defs.h"

#define EDITOR_MAIN 0
#define EDITOR_BRICKS_SELECTION 1

static playField pf;
static cursorType cur;
static int selBrick=1;
static int changed=0;
static char fileName[64];
static char buf[64];
static int teleState=0; //Teleport placement iteration
static int teleSrcPos[2];
static int editorState;
SDL_Surface* selBrickBG;
spriteType* selBrickBgSprite;

void editorLoad(const char* fn, SDL_Surface* screen)
{
  //Set filename
  editorFileName(fn);

  //Init cursor
  initCursor(&cur);
  //Load field
  loadField(&pf, fileName);
  //Read info's for level.
  pf.levelInfo = mkLevelInfo( fn );

  initDraw(pf.levelInfo, screen);
  SDL_FreeSurface(stealGfxPtr()->boardImg);
  stealGfxPtr()->boardImg = loadImg( DATADIR"data/editbg.png" );
  selBrickBG = loadImg( DATADIR"data/editselbrick.png" );
  selBrickBgSprite = cutSprite( selBrickBG, 0,0, selBrickBG->w, selBrickBG->h );
  changed=0;
  selBrick=BRICKSBEGIN;

  teleState=0;

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
  free(selBrickBgSprite);
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

void editorRemoveBrickUnderCursor()
{
  if(pf.board[cur.x][cur.y])
  {
    free(pf.board[cur.x][cur.y]);
    pf.board[cur.x][cur.y]=0;
  }

  //teleport?
  teleRemoveFromList(pf.levelInfo->teleList,cur.x,cur.y);

  boardSetWalls(&pf);
  changed=1;
}


int runEditor(SDL_Surface* screen)
{
  SDL_Rect selBrickRect;

  if( editorState == EDITOR_MAIN )
  {
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

    if( (getButton(C_BTNX) || getInpPointerState()->isDown ) && selBrick != RESERVED )
    {
      //resetBtn(C_BTNX);
      //resetMouseBtn();

      //We remove the brick under the cursor if it's not a teleport, or if it is and we are placing a teleport source.
      if( (!telePresent(pf.levelInfo->teleList, cur.x, cur.y ) && selBrick!=TELESRC) || (selBrick==TELESRC && telePresent(pf.levelInfo->teleList, cur.x, cur.y ) && teleState==0 ) )
      {
        editorRemoveBrickUnderCursor();
      }

      if(selBrick==TELESRC)
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
          teleAddToList( pf.levelInfo->teleList, teleSrcPos[0], teleSrcPos[1], cur.x, cur.y );
          //Reset state
          teleState=0;
        }
      } else {
        pf.board[cur.x][cur.y]=malloc(sizeof(brickType));

        pf.board[cur.x][cur.y]->type=selBrick;
        boardSetWalls(&pf);
        pf.board[cur.x][cur.y]->pxx=cur.x*20+boardOffsetX;
        pf.board[cur.x][cur.y]->pxy=cur.y*20+boardOffsetY;
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

    if(getButton(C_BTNSELECT))
    {
      resetBtn(C_BTNSELECT);
      FILE *f = fopen(fileName, "w");
      int x,y;
      sprintf(buf, "#Author of level\nauthor=%s\n\n", pf.levelInfo->author);
      fputs(buf,f);

      sprintf(buf, "#Name of the level\nlevelname=%s\n\n", pf.levelInfo->levelName);
      fputs(buf,f);

      sprintf(buf, "#Seconds to complete level\nseconds=%i\n\n", pf.levelInfo->time);
      fputs(buf,f);

      sprintf(buf, "bgfile=%s\n", pf.levelInfo->bgFile);
      fputs(buf,f);

      sprintf(buf, "tilebase=%s\n", pf.levelInfo->tileBase);
      fputs(buf,f);

      sprintf(buf, "explbase=%s\n", pf.levelInfo->explBase);
      fputs(buf,f);

      sprintf(buf, "wallbase=%s\n", pf.levelInfo->wallBase);
      fputs(buf,f);

      sprintf(buf, "sounddir=%s\n", pf.levelInfo->soundDir);
      fputs(buf,f);

      sprintf(buf, "charbase=%s\n", pf.levelInfo->fontName);
      fputs(buf,f);

      sprintf(buf, "cursorfile=%s\n", pf.levelInfo->cursorFile);
      fputs(buf,f);

      sprintf(buf, "startimage=%s\n", (pf.levelInfo->startImg)?pf.levelInfo->startImg:"none");
      fputs(buf,f);

      sprintf(buf, "stopimage=%s\n", (pf.levelInfo->stopImg)?pf.levelInfo->stopImg:"none");
      fputs(buf,f);

      //Teleports
      char* str = teleMkStrings(pf.levelInfo->teleList);
      if(str) //Returns 0 if there's no teleports
      {
        fputs("\n#Teleports\n",f);
        fputs(str,f);
        free(str);
      }


      fputs("\n#The level-data block\n[data]",f);

      if(f)
      {
        for(y=0; y < FIELDSIZE; y++)
        {
          fputc('\n',f);
          for(x=0; x < FIELDSIZE; x++)
          {
            if(pf.board[x][y])
            {
              fprintf(f,"%02i", pf.board[x][y]->type);
            } else {
              fprintf(f,"00");
            }
          }
        }
        fputc('\n',f);
        changed=0;
        fclose(f);

        //Refresh the list of userLevels.
        addUserLevel(fileName);
      }

    }

    if(getButton(C_BTNMENU))
    {
      resetBtn( C_BTNMENU );
      changed++; //If it was 0 then it will become 1 (saved) exit. If it was 1 it becomes 2 (not saved).
      if( changed != 2 )
      {
        editorCleanUp();
        return(STATEMENU);
      }
    }

  } //Editor in main state, don't ignore imput


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
    txtWriteCenter(screen, FONTSMALL, "(To)", HSCREENW-115,HSCREENH-41);
    drawPath(screen, teleSrcPos[0], teleSrcPos[1], cur.x, cur.y, 1);
  }
  drawAllTelePaths(screen, pf.levelInfo->teleList);

  //Draw brick-selection
  if( editorState == EDITOR_BRICKS_SELECTION )
  {
    //Draw box for the bricks "24 px"
//    drawSprite( screen, selBrickBgSprite, HSCREENW-78, HSCREENH-42 );
    SDL_BlitSurface(selBrickBG , NULL, screen, &(setting()->bgPos) );

    //Draw a 3*6 grid
    int px,py,bnum=BRICKSBEGIN;
    static int brickSelOfX = HSCREENW - 78 + 8;
    static int brickSelOfY = HSCREENH - 42 + 8;
    for(py=0;py < 3; py++)
    {
      for(px=0; px < 6; px++)
      {
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


