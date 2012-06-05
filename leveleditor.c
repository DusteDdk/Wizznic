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

static playField pf;
static cursorType cur;
static int selBrick=1;
static int changed=0;
static char fileName[64];
static char buf[64];
static int teleState=0; //Teleport placement iteration
static int teleSrcPos[2];

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
  changed=0;
  selBrick=1;

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
}

void editorFileName(const char* fn)
{
  changed=1;
  strcpy(fileName,fn);
}

void editorRemoveBrickUnderCursor()
{
  if(pf.board[cur.x][cur.y])
  {
    free(pf.board[cur.x][cur.y]);
    pf.board[cur.x][cur.y]=0;
    changed=1;
  }

  //teleport?
  teleRemoveFromList(pf.levelInfo->teleList,cur.x,cur.y);

  boardSetWalls(&pf);
}


int runEditor(SDL_Surface* screen)
{

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

  if(getButton(C_BTNX))
  {
    resetBtn(C_BTNX);

    //If it's empty and we are not placing a teledestination, remove brick at cursor
    if(pf.board[cur.x][cur.y] && !(selBrick==TELESRC && teleState!=0) )
    {
      editorRemoveBrickUnderCursor();
    }

    if(selBrick==TELESRC)
    {
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

  if(getButton(C_BTNY))
  {
    resetBtn(C_BTNY);
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


  //Write brick name.
  txtWriteCenter(screen, FONTSMALL, str_brick_names[selBrick], HSCREENW-116,HSCREENH-65+9 );

  drawBrick(screen, selBrick,HSCREENW-125,HSCREENH-85);
  txtWriteCenter(screen, FONTSMALL, STR_EDIT_BRICK,HSCREENW-115,HSCREENH-100);
  #ifdef GP2X
  txtWrite(screen, FONTSMALL,"<A", 35-18-3,40);
  txtWrite(screen, FONTSMALL,"B>", 55+3,40);
  #elif defined (PSP)
  txtWrite(screen, FONTSMALL,"X", 100,56);
  txtWrite(screen, FONTSMALL,"O", 140,56);
  #elif defined (PANDORA)
  txtWrite(screen, FONTSMALL,"B>", HSCREENW-160,HSCREENH-80);
  txtWrite(screen, FONTSMALL,"<A", HSCREENW-102,HSCREENH-80);
  #else
  txtWrite(screen, FONTSMALL,"Ctrl", HSCREENW-160,HSCREENH-80);
  txtWrite(screen, FONTSMALL,"Alt", HSCREENW-102,HSCREENH-80);
  #endif

  if(teleState)
  {
    txtWrite(screen, FONTSMALL, "Place\nDestination", HSCREENW-160,HSCREENH-50);
    drawPath(screen, teleSrcPos[0], teleSrcPos[1], cur.x, cur.y, 1);
  }
  drawAllTelePaths(screen, pf.levelInfo->teleList);


  return(STATEEDIT);
}


