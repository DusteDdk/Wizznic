#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define FIELDSIZE 11

//Empty space
#define TRANSBG 0     //if ptr = 0

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


#define ONEWAYSPEED 10

#define DIRLEFT -1
#define DIRRIGHT 1

#define DIRUP -1
#define DIRDOWN 1

//Screen sizes and their halves.
#ifdef PSP
  #define SCREENW 480
  #define SCREENH 272
  #define HSCREENW 240
  #define HSCREENH 136
#else
  #define SCREENW /*640*/320
  #define SCREENH /*480*/240
  #define HSCREENW  /*320*/160
  #define HSCREENH  /*240*/120
#endif


//Board offsets
/*#ifdef PSP
  #define boardOffsetX 170
  #define boardOffsetY 26
#else
  #define boardOffsetX 90
  #define boardOffsetY 10
#endif
*/
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

#define FRAMERATE 50

//To avoid crashing a new version trying to read old highscore files (hmm, as if it's ever gonna happen)
#define STATS_FILE_FORMAT_VERSION 1

//Url where stats are
#define STATS_SERVER_URL "http://dusted.dk/wizznic"
#ifdef WIN32
  #define CURLBIN "curl\\curl.exe --user-agent wizznicWindows --connect-timeout 10 --fail --silent --url "STATS_SERVER_URL"/commit.php --data-ascii "
#else
  #define CURLBIN "wget "STATS_SERVER_URL"/commit.php -O - -q --user-agent=wizznicLinux --timeout=10 --tries=1 --post-data="
#endif


#endif // DEFS_H_INCLUDED
