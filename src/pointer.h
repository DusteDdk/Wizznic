#ifndef POINTER_H_INCLUDED
#define POINTER_H_INCLUDED

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

typedef struct {
  int startX, startY; //position it started to be pressed on the board
  int curX, curY;     //Position on the board (11x11 positions)
  int vpX, vpY;       // "virtual"/scaled pixel positions (320x230)ish
  int downTime;       //Milliseconds curser have been pressed down, is 0 first iteration
  int timeSinceMoved; //Milliseconds since cursor last moved, is 0 first iteration
  int isDown;         //Is 1 if the pointer is pressed down
  int_fast8_t hitABox; //Set0 each click, set1 if isBoxClicked returns true
  int_fast8_t escEnable;// If 1, the "escape" box is shown in the upper corner
  uint32_t colWhite;   //Color of dots in cursor
} inpPointerState_t;

inpPointerState_t* getInpPointerState();

int_fast8_t isPointerClicked(); //This does not take into account if the pointer actually hit anything
int_fast8_t isAnyBoxHit(); //This will return true if any of the calls to isBoxClicked did this tick.
int_fast8_t isPointerInBox( SDL_Rect* r );
int_fast8_t isBoxClicked( SDL_Rect* r );

int_fast8_t isPointerEscapeClicked(); //Returns 1 if the "escbox" was hit and escEnable=1;

void initPointer(SDL_Surface* screen);
void drawPointer(SDL_Surface* screen);

#endif // POINTER_H_INCLUDED
