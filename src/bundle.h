#ifndef BUNDLE_H_INCLUDED
#define BUNDLE_H_INCLUDED

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

#define BUNDLE_SUCCESS 1
#define BUNDLE_FAIL_DIR_EXISTS 2
#define BUNDLE_FAIL_CORRUPT 4
#define BUNDLE_FAIL_NO_WRITE_PERMISSION 8
#define BUNDLE_FAIL_COULD_NOT_OPEN 16
#define BUNDLE_FAIL_UNSUPPORTED_VERSION 32
#define BUNDLE_FAIL_NOT_BUNDLEFILE 64

#define BUNDLE_FILE_VERSION 0x11

int debundle( const char* file, const char* outDir );
void bundle( const char* file, const char* inDir);
//Get a string telling the path to the last extracted bundle (or NULL if no bundle was extracted).
const char* bundlePath();
//Frees the bundle-path and sets it to NULL
void bundlePathReset();
int getBundleError();
void bundleSetFlip(uint_fast8_t flip);

#endif
