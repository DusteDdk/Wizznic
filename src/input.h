#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

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
#include "pointer.h"
#include "board.h"

#define C_UP 0
#define C_DOWN 1
#define C_LEFT 2
#define C_RIGHT 3
#define C_BTNY 4
#define C_BTNX 5
#define C_BTNA 6
#define C_BTNB 7
#define C_SHOULDERA 8
#define C_SHOULDERB 9
#define C_BTNMENU 10
#define C_BTNSELECT 11
#define C_BTNVOLUP 12
#define C_BTNVOLDOWN 13
#define C_NUM 14

#ifndef PLATFORM_JOY_DEADZONE
#define PLATFORM_JOY_DEADZONE 500
#endif

SDLKey getChar();
void setButton(int btn); //Nasty
int getButton(int btn);
int getBtnTime(int btn);
void resetBtnTimer(int btn);
void resetBtn(int btn);
void resetMouseBtn();
void resetBtnAll();
int runControls();
void initControls();

#endif // INPUT_H_INCLUDED
