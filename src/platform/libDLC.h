#ifndef LIBDLC_H_INCLUDED
#define LIBDLC_H_INCLUDED

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

//Ready for action
#define DLC_READY 1
//Downloading
#define DLC_BUSY 2
//Download succeeded, file extracted
#define DLC_INSTALLED 3
//Nope
#define DLC_FAILED 4
//The api is outdated
#define DLC_API_OUTDATED 5
//Some error happened while debundle.
#define DLC_BUNDLE_ERROR 6

//Tries to install a DLC using "code"
void dlcTryInstall( const char* code, const char* dest );

//Get the DLC_* state
int dlcGetState();

//Set DLC to ready state
void dlcSetReady();

//Start thread to check if the DLC server can be reached, and if we have the right api version.
void dlcCheckOnline();

//Upload a user-level
#ifdef PLATFORM_SUPPORTS_STATSUPLOAD
int dlcPushFile( const char* fileName, char* msg );
#endif

#endif
