#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

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
#include "ticks.h"
#include "input.h"
#include "defs.h"
#include "sound.h"
#include "board.h"
#include "draw.h"
#include "cursor.h"
#include "sprite.h"
#include "text.h"
#include "states.h"
#include "player.h"
#include "menu.h"
#include "pack.h"

int initGame(SDL_Surface* screen);
void cleanUpGame();
int runGame(SDL_Surface* screen);
void setGameState(int state);

#endif // GAME_H_INCLUDED
