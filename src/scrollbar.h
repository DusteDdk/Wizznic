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

#ifndef SCROLLBAR_H_
#define SCROLLBAR_H_

#include <SDL/SDL.h>

void scrollBarInit();

//Returns current position if a change happened, or -1 if nothing happened.
//size is the number of items in the list, current is the currently selected item, chdir is -1 for up and +1 for down (for blinking when arrowkeys are pressed)
int scrollBar(SDL_Surface* screen, int x, int y, float size, int current, int chdir);

#endif /* SCROLLBAR_H_ */
