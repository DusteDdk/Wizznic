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

#include <stdio.h>
#include <string.h>

#include "about.h"
#include "scrollbar.h"
#include "list/list.h"
#include "defs.h"
#include "text.h"

static char** lines;
int numLines;

void aboutInit()
{
  char buf[128];
  list_t* txt;
  listItem* it;
  FILE* fp = fopen( DATADIR"data/about.txt", "r");

  if(!fp)
  {
    printf("Couldn't open "DATADIR"data/about.txt\n");
       return;

  }
  txt = listInit(NULL);

  while( fgets(buf, 128, fp ) )
  {
    int l = strlen(buf);
    it = listAppendData(txt, malloc( l+1 ) );
    strcpy( (char*)(it->data), buf );
  }

  numLines = txt->count;
  lines = malloc( sizeof(char*)*numLines );
  listAddToArray((void*)lines,txt);
  listFree(txt);
}

//Return number of lines of text
int aboutScreen(SDL_Surface* screen, int* menuPosY,  int menuChangeY)
{
  int i;
  for(i=*menuPosY; i < 23+*menuPosY && i < numLines; i++)
    txtWrite(screen, FONTSMALL, lines[i], HSCREENW-160+28, HSCREENH-120+40+(i-*menuPosY)*8);

  i=scrollBar(screen, HSCREENW-160+6,HSCREENH-120+40, numLines-15, *menuPosY,menuChangeY);
  if( i != -1 )
  {
    *menuPosY=i;
  }

  return(numLines-15);
}
