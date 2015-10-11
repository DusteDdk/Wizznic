#ifndef DUMPLEVELIMAGES_H_INCLUDED
#define DUMPLEVELIMAGES_H_INCLUDED

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
    uint8_t* data;
    int len;
} tgaData_t;

void dumplevelimages(SDL_Surface* screen, const char* packName, int dumpStartImage);
void dumpOneLevelFile(SDL_Surface* screen, const char* fileName);
void tgaSave(tgaData_t* tga, const char* fileName);
tgaData_t* tgaData(SDL_Surface* screen);
void tgaFree(tgaData_t* tga);

void screenShotSetCaptureScreen(SDL_Surface* screen);
void screenShot();

#endif // DUMPLEVELIMAGES_H_INCLUDED
