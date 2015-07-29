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

#include "strinput.h"
#include "input.h"
#include "strings.h"

static char kbl[4][10] = {
  {'1','2','3','4','5','6','7','8','9','0'},
  {'q','w','e','r','t','y','u','i','o','p'},
  {'a','s','d','f','g','h','j','k','l',';'},
  {'z','x','c','v','b','n','m',' ',',','.'}
};

static char kbh[4][10] = {
  {'!','"','#','$','%','&','/','(',')','='},
  {'Q','W','E','R','T','Y','U','I','O','P'},
  {'A','S','D','F','G','H','J','K','L',':'},
  {'Z','X','C','V','B','N','M','_',',','.'}
};

static int kbRows = 4;
static int kbCols = 10;




//Return a state structure for use with inpGetStr and inpGetStrClear
inpStrState* inpStrCreate(SDL_Surface* dstSurface, char* promptTxt, size_t minLen, size_t maxLen )
{
  inpStrState* s = malloc( sizeof(inpStrState) );
  s->dstSurface=dstSurface;
  s->maxLen=maxLen;
  s->minLen=minLen;
  s->prompt=promptTxt;

  s->str=malloc(sizeof(char)*(maxLen+1));
  s->_buf=malloc( (sizeof(char))*(maxLen+strlen(s->prompt)+2) );

  inpStrClear(s);
  return(s);
}

void inpStrClear(inpStrState* state)
{
  memset(state->str, 0, (state->maxLen+1) );
  state->curX=0;
  state->curY=0;
  state->curPos=0;
  state->kb=&kbl;
}

//Return 0 until a string is present in state->str.
int inpStrGet(inpStrState* state, int menuPosX, int menuPosY, int blink)
{
  int cy,cx;
  char hack[2]={ ' ',0x00 };
  int hsKeyboardWasClicked=0;

  txtWriteCenter(state->dstSurface, FONTSMALL, STR_STRINPUT_CONTROLS, HSCREENW,HSCREENH+108);

  if( getChar() )
  {
    if( getChar() == SDLK_RETURN )
    {
      //Save
      menuPosY=4;
      hsKeyboardWasClicked=1;
    }

    if( getChar() == SDLK_BACKSPACE && strlen( state->str ) > 0 )
    {
      state->str[ strlen(state->str)-1 ]=0;
    } else if( getChar() > 31 && getChar() < 123 && strlen( state->str ) < state->maxLen+1 )
    {
      state->str[ strlen(state->str) ] = getChar();
      state->str[ strlen(state->str)+1 ] = 0x0;
    }
  }

  sprintf(state->_buf, "%s %s", state->prompt, state->str);

  txtWrite(state->dstSurface, FONTSMALL, state->_buf, HSCREENW-110, HSCREENH-45);


  for(cy=0; cy < kbRows; cy++)
  {
    for(cx=0; cx < kbCols; cx++)
    {

      if( (menuPosX==cx && menuPosY==cy) && !blink && !( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT  ) )
      {
        txtWrite(state->dstSurface, FONTSMALL, "[_]", (HSCREENW-70)+(cx)*16-8, (HSCREENH-9)+cy*15);
      }

      hack[0]=(*(state->kb))[cy][cx];
      txtWrite(state->dstSurface, FONTSMALL, hack, (HSCREENW-70)+cx*16, (HSCREENH-10)+cy*15);
      hack[0]=0;
      if( isBoxClicked( getTxtBox() ) )
      {
        menuPosX=cx;
        menuPosY=cy;
        hsKeyboardWasClicked=1;
      }
    }
  }

  //Only show "ENTER" if string nonempty
  if( strlen(state->str) )
  {
    //Blink "ENTER" underline if selected
    if( menuPosY==4 && blink)
    {
      if(menuPosX > 9) menuPosX=0;
      txtWriteCenter(state->dstSurface, FONTSMALL, " _____ ", HSCREENW, HSCREENH+50);
    }
    txtWriteCenter(state->dstSurface, FONTSMALL, "[ENTER]", HSCREENW, HSCREENH+50);
    if( isBoxClicked( getTxtBox() ) )
    {
      menuPosY=4;
      hsKeyboardWasClicked=1;
    }
  }

  //Blink "Caps" underline if selected
  if( menuPosX==10 && blink)
  {
    if(menuPosY >3) menuPosY =1;
    txtWrite( state->dstSurface, FONTSMALL, " ____", HSCREENW-130, HSCREENH+10 );
  }
  txtWrite( state->dstSurface, FONTSMALL, "[Caps]", HSCREENW-130, HSCREENH+10 );
  if( isBoxClicked( getTxtBox() ))
  {
    menuPosX=10;
    hsKeyboardWasClicked=1;
  }

  //If the cursor is being used, show "delete" button, this is only usable with pointer.
  if(  getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT  )
  {
    txtWrite(state->dstSurface, FONTSMALL, "[DEL]", (HSCREENW+38), (HSCREENH-25) );
    if( isBoxClicked( getTxtBox() ))
    {
      menuPosY=5;
      hsKeyboardWasClicked=1;
    }
  }

  //Switch layouts if we're at posX 10
  if( menuPosX == 10 && (getButton( C_BTNB ) || hsKeyboardWasClicked) )
  {

    resetBtn( C_BTNB );
    if(state->kb==&kbl)
    {
      state->kb=&kbh;
    } else if(state->kb==&kbh) {
      state->kb=&kbl;
    }

  } else if( menuPosY==4 && ( getButton( C_BTNB ) || hsKeyboardWasClicked ) ) //Return true
  {
    resetBtn( C_BTNB );
    if(strlen( state->str ) >= state->minLen)
    {
      return(1);
    }
  } else if( getButton( C_BTNB ) || (hsKeyboardWasClicked && menuPosY < 5) )
  {
    resetBtn( C_BTNB );
    resetMouseBtn();
    if( strlen( state->str ) < state->maxLen+1)
    {
      state->str[ strlen(state->str) ] = (*(state->kb))[menuPosY][menuPosX];
      state->str[ strlen(state->str)+1 ] = 0;
    }
  } else if( getButton( C_BTNA ) || (hsKeyboardWasClicked && menuPosY == 5) )
  {
    resetBtn( C_BTNA);
    if(strlen(state->str) > 0)
    {
      state->str[ strlen(state->str)-1 ]=0;
    }
  }
  return(0);
}
