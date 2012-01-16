#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

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

#define FIELDSIZE 11

//Bricks
#define BRICKSBEGIN 1 //The first brick
#define BRICKSEND 10  //The last brick

//Tiles
#define MOVERVERT 11  //Vertical (up-down) moving platform
#define MOVERHORIZ 12 //Horizontal (left-right) moving platform
#define ONEWAYLEFT 13 //Bricks standing directly on this can only be moved left
#define ONEWAYRIGHT 14//Only moved right
#define GLUE 15       //Can't be moved at all
#define STDWALL 16    //Standard issue free-standing wall-tile
#define RESERVED 17   //Reserved for bricks that are moving/exploding.
#define TELESRC 18    //Teleport source brick

#define NUMTILES 18 //type 1 = index 0 in tile array

#define MOVERCOUNTDOWN 500  //Ms pause before mover switch direction

//Milliseconds we show the small pointer before hiding it again (in case people don't want to use mouse anyway)
#define POINTER_SHOW_TIMEOUT 10000

#define ONEWAYSPEED 10

#define DIRLEFT -1
#define DIRRIGHT 1

#define DIRUP -1
#define DIRDOWN 1

//Data directory
#ifndef DATADIR
  #define DATADIR "./"
#endif

//Get defines from platform .h file
#if defined(PANDORA)  //Pandora
  #include "platform/pandora.h"
#elif defined(PSP)    //PSP
  #include "platform/pspspec.h"
#elif defined(GP2X)   //GP2X
  #include "platform/gp2x.h"
#elif defined(WIZ)    //GP2X Wiz
  #include "platform/wiz.h"
#else                 //Linux, Windows, Default
  #include "platform/pc.h"
#endif


//Half the resolution is practical for centering content
#define HSCREENW  SCREENW/2
#define HSCREENH  SCREENH/2


//Board offsets
#define boardOffsetX 90 + ( HSCREENW - 160 )
#define boardOffsetY 10 + ( HSCREENH - 120 )

//Board sizes
#define brickSize 20
#define NOBLOCK 0
#define DOBLOCK 1
#define CURLOCK 2

//Speeds
#define CURSORMOVESPEED 5
#define VERTMOVERSPEED 1
#define HORIZMOVERSPEED 1
#define FALLINGSPEED 2

//Delays
#define REPEATDELAY 200

//Particle system layer
#define PSYS_LAYER_NODRAW 0
#define PSYS_LAYER_TOP 1
#define PSYS_LAYER_UNDERBRICK 2
#define PSYS_LAYER_UNDERDEATHANIM 3

//To avoid crashing a new version trying to read old highscore files (hmm, as if it's ever gonna happen)
#define STATS_FILE_FORMAT_VERSION 1

//Url where stats are
#define STATS_SERVER_URL "http://dusted.dk/wizznic"

#ifndef STR_PLATFORM
  #ifndef STR_PLATFORM
    #define STR_PLATFORM "Unknown"
  #endif
#endif

#endif // DEFS_H_INCLUDED
