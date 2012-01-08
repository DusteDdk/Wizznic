#ifndef HEADER_47EFC508C43A8437
#define HEADER_47EFC508C43A8437

/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2011 Jimmy Christensen <dusted@dusted.dk>             *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#ifndef LEVELEDITOR_H_INCLUDED
#define LEVELEDITOR_H_INCLUDED

#include <SDL/SDL.h>


void editorCleanUp();
void editorLoad(const char* fileName, SDL_Surface* screen);
void editorFileName(const char* fileName);
int runEditor(SDL_Surface* screen);

#endif // LEVELEDITOR_H_INCLUDED

#endif // header guard 
