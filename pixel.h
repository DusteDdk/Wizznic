#ifndef HEADER_C85983A772BA7658
#define HEADER_C85983A772BA7658

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

#ifndef PIXEL_H_INCLUDED
#define PIXEL_H_INCLUDED

#include <SDL/SDL.h>

inline void plotPixel(SDL_Surface* img, int x, int y, uint32_t col);
inline void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col);
inline uint32_t freadPixel(SDL_Surface* img, int x, int y);

void setAlphaCol( int bpp );
inline int_fast8_t isAlpha(int_fast8_t r, int_fast8_t g, int_fast8_t b);

#endif // PIXEL_H_INCLUDED

#endif // header guard
