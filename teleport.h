#ifndef TELEPORT_H_INCLUDED
#define TELEPORT_H_INCLUDED

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
 * along with Wizznic.  If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

#include <SDL/SDL.h>
#include "list.h"

struct telePort_s {
  int sx, sy;
  int dx, dy;
};
typedef struct telePort_s telePort_t;

void teleFreeList( listItem* l );
void teleAddToList( listItem* l, int sx, int sy, int dx, int dy );
void teleRemoveFromList( listItem* l, int sx, int sy );
void teleAddFromString(listItem* l, const char* str); //Expects a string in the format sx,sy:dx,dy
char* teleMkStrings(listItem* l, const char* prefix);
int_fast8_t telePresent( listItem* l, int sx,int sy);

#endif // TELEPORT_H_INCLUDED
