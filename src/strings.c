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
#include <string.h>
#include <stdint.h>

#include "strings.h"

// We reuse the "Reserved/blocker" brick as a "Remove" icon in the mousedriven editor.
const char* str_brick_names[] = { "Nobrick", "Brick 0", "Brick 1","Brick 2","Brick 3","Brick 4","Brick 5","Brick 6","Brick 7","Brick 8","Brick 9","V. Lift", "H. Lift", "L. Oneway", "R. Oneway", "Glue", "Wall", "Remove", "Teleport", "SwOn", "SwOff", "Rem.Brick", "Cpy.Brick", "Game Over", "BrickType" };

void stripNewLine(char* str)
{
  int i;
  int strLen = (int)strlen(str);
  for(i=0; i<strLen; i++)
  {
    if( str[i] == '\r' || str[i] == '\n' )
      str[i]='\0';
  }
}

int splitVals(char ch,const char* buf, char* set, char* val)
{
  int strpos=0;
  int strLen = (int)strlen(buf);
  set[0]='\0';
  val[0]='\0';
  if(buf[0]!='#')
  {
    for(strpos=0; strpos < strLen;strpos++)
    {
      //Here's a setting!
      if(buf[strpos]==ch)
      {
        strncpy(set,buf,strpos+1);
        set[strpos]='\0';
        strcpy(val,buf+strpos+1);
        return(1);
      }
    }
  }
  return(0);
}

//Return position of last instance of c
//Return 0 if none found
int charrpos(const char* str, char c)
{
  int i;
  for(i=strlen(str); i != 0; i--)
  {
    if(str[i]==c)
    {
      return(i);
    }
  }
  return(0);
}
