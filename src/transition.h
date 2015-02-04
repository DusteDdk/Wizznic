#ifndef TRANSITION_H_INCLUDED
#define TRANSITION_H_INCLUDED

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

#include <SDL/SDL.h>

#define TRANSITION_TYPE_DISSOLVE 0
#define TRANSITION_TYPE_CURTAIN_UP 1
#define TRANSITION_TYPE_CURTAIN_DOWN 2
#define TRANSITION_TYPE_ROLL_OUT 3
#define TRANSITION_TYPE_ROLL_IN 4
#define NUM_TRANSITIONS 5

#define TRANSITION_TYPE_RANDOM 255

void initTransition();
void startTransition(SDL_Surface* scr, uint_fast8_t type, uint_fast16_t time);
void runTransition(SDL_Surface* scr);

#endif
