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

#include <unistd.h>
#include "strings.h"
#include "stats.h"
#include "pack.h"
#include "defs.h"
#include "player.h"
#include "settings.h"
#include "userfiles.h"

static stats_t st;

//Return ptr to stats_t
stats_t* stats()
{
  return(&st);
}

//Sets some ptrs 0, that's all.
void statsInit()
{
  memset( &st, 0, sizeof(stats_t) );
  printf("statsInit(); Stats ready to be refreshed.\n");
}

//Tries to load stats
void statsLoad()
{
  FILE* f;
  char* buf = malloc(sizeof(char)*128);
  char* bufb = malloc(sizeof(char)*128);
  int i;
  hsEntry_t* hs;
  hsEntry_t ths;
  statsFileHeader_t sfh;
  listItem* it;

  //Set progress -1 before reading the real progress from file (in case there is no file yet)
  // -1 because progress is updated to current-level after the completion of that level, but
  // at the same time, it is used (with +2) to set maxY in menu.
  st.progress=-1;

  //Free levelStats if allready filled
  if(st.levelStats)
  {
    listFree( st.levelStats );
  }

  //Create new list for levelStats
  st.levelStats = listInit(free);
  for(i=0; i < packState()->cp->numLevels; i++)
  {
    hs = malloc(sizeof(hsEntry_t));
    //Set name 0, we're not going to use it for levelStats
    memset( hs->name, 0, 11 );
    //Set default "best"
    hs->levelNum=i;
    hs->score=0;
    hs->time=9999;
    hs->moves=9999;
    hs->combos=0;
    //Add to list
    listAppendData(st.levelStats, (void*)hs);
  }

  //Remove packwide highscores if they exist
  if(st.packHsTable)
  {
    listFree(st.packHsTable);
  }

  //Create new list for packWide highscores
  st.packHsTable = listInit(free);


  //Cut pack name  out of pack-path from last /
  strcpy( buf, packState()->cp->path+charrpos(packState()->cp->path, '/')+1 );
  //prepend the highscore path
  sprintf(bufb, "%s/%s.hig", getHighscoreDir(), buf);

  //Copy name to st.
  if(!st.hsFn)
    free(st.hsFn);
  st.hsFn = malloc(sizeof(char)*strlen(bufb)+1);
  strcpy(st.hsFn, bufb);
  f=fopen(st.hsFn, "r");


  if(f)
  {
    //Read the int and check that it's the version we expect.
    size_t elementsRead = fread( &i, sizeof(int),1,f);

    if( elementsRead == 1 && i == STATS_FILE_FORMAT_VERSION )
    {
      //Jump to start of file again
      rewind(f);
      //Read the header
      size_t elementsRead = fread( (void*)(&sfh), sizeof(statsFileHeader_t), 1, f);
      if( elementsRead != 1 )
      {
        printf("Something went wrong while trying to read '%s'\n",st.hsFn);
      }
      //Set progress
      st.progress = sfh.progress;

      //Override defaults for levels found in file
      for(i=0; i < sfh.numLevelEntries; i++)
      {
        //Read a highscore entry
        elementsRead = fread( (void*)(&ths), sizeof(hsEntry_t), 1, f );

        //Look for a list to add it to
        if( ths.levelNum <  st.levelStats->count )
        {
          it = listGetItemAt(st.levelStats,ths.levelNum);

          //If it was found, copy it
          if(elementsRead==1 && it)
          {
            hs = (hsEntry_t*)it->data;
            memcpy( hs, &ths, sizeof(hsEntry_t) );
          }
        } else {
          printf("That's odd, there's an entry for level %i even though there's only %i levels in the pack. (levelStats is %i)\n", ths.levelNum, getNumLevels(), st.levelStats->count );
        }
      }



      //Fill in the pack-wide highscore list
      for(i=0; i < sfh.numHsEntries; i++)
      {
        hs=malloc(sizeof(hsEntry_t));
        elementsRead = fread(hs,sizeof(hsEntry_t),1,f);
        if( elementsRead == 1 )
        {
          listAppendData(st.packHsTable, (void*)hs);
        }
      }

      fclose(f);
    } else {

      //TODO: This is not how we will handle it, if the format ever changes we will use version info to migrate to new format.
      printf("File '%s' is version %i but current version is %i, deleting the file.\n", st.hsFn, i, STATS_FILE_FORMAT_VERSION);
      fclose(f);
      packUnlinkHsFile();
    }
  }

  free(buf);
  free(bufb);
}

void statsSave()
{
  listItem* it;
  hsEntry_t* hs;

  //Check that there's a filename to write to (st is all 0's if not)
  if(!st.hsFn)
  {
    printf("statsSave(); Fatal error; Called to save stats, but there's no filename to save.\n");
    return;
  }

  //Open file
  FILE* f = fopen(st.hsFn, "w");
  if(f)
  {
    //Fill in header
    statsFileHeader_t sfh;
    sfh.hsFileVersion = STATS_FILE_FORMAT_VERSION;
    sfh.progress = st.progress;
    sfh.numLevelEntries = st.levelStats->count;
    sfh.numHsEntries  = st.packHsTable->count;

    //Write header
    fwrite( (void*)(&sfh), sizeof(statsFileHeader_t),1, f );

    //Write levelstats
    it=&st.levelStats->begin;
    while( LISTFWD(st.levelStats, it) )
    {
      hs=(hsEntry_t*)it->data;
      //Write entry to file.
      fwrite( (void*)(hs), sizeof(hsEntry_t), 1, f);
    }

    //Write packWide hstable
    it = &st.packHsTable->begin;
    while( LISTFWD(st.packHsTable, it) )
    {
      hs = (hsEntry_t*)it->data;
      fwrite( (void*)(hs), sizeof(hsEntry_t), 1, f);
    }

    fclose(f);
  } else {
    printf("Error, couldn't open '%s' for writing.\n", st.hsFn);
  }

}

//Figure out if this deserves an entry in the highscore table, update number of games played
void statsSubmitBest()
{
  hsEntry_t* lvlHs = &player()->hsEntry;

  statsSetLevel(lvlHs->levelNum);
  lvlHs->time /= 1000;

  //Update the campain-wide scores
  player()->campStats.levelNum = player()->level;

  player()->campStats.score += player()->hsEntry.score;

  player()->campStats.time += lvlHs->time;

  player()->campStats.moves += lvlHs->moves;

  player()->campStats.combos += lvlHs->combos;

  statsUpload(player()->level, lvlHs->time, lvlHs->moves,lvlHs->combos,lvlHs->score, "complete",0, NULL);

  //Update progress, if any.
  if( lvlHs->levelNum > st.progress )
    st.progress=lvlHs->levelNum;

 /* printf("----\nstatsSubmitBest; Current best stats for level %i\n"
         "Score: %i\n"
         "Time: %i\n"
         "Moves: %i\n"
         "Combos: %i\n", st.cl->levelNum,st.cl->score,
         st.cl->time,
         st.cl->moves,
         st.cl->combos);*/


  //Now override the stats that are better in hs.
  if(lvlHs->score > st.cl->score)
  {
    st.cl->score = lvlHs->score;
  }

  if(lvlHs->time < st.cl->time)
  {
    st.cl->time=lvlHs->time;
  }

  if(lvlHs->moves < st.cl->moves)
  {
    st.cl->moves = lvlHs->moves;
  }

  if(lvlHs->combos > st.cl->combos)
  {
    st.cl->combos = lvlHs->combos;
  }

  statsSave();

}

void statsSetLevel(int l)
{
  st.cl=(hsEntry_t*)listGetItemAt(st.levelStats, l)->data;
}

void statsDrawHs(SDL_Surface* screen)
{
  listItem* it = &stats()->packHsTable->begin;
  hsEntry_t* h;
  int i=0;
  char buf[64];

  txtWriteCenter(screen, FONTMEDIUM,"Highscores!", HSCREENW, HSCREENH-105);

  txtWriteCenter(screen, FONTSMALL, packState()->cp->name, HSCREENW, HSCREENH-80);

  txtWriteCenter(screen, FONTSMALL, "L Name      Score  Time  Move Combo", HSCREENW,HSCREENH-50);
  txtWriteCenter(screen, FONTSMALL, "+-+---------+------+-----+----+----", HSCREENW,HSCREENH-40);

  while( LISTFWD(stats()->packHsTable, it) )
  {
    h= (hsEntry_t*)it->data;
    sprintf(buf, "%-2i%-10s%-7i%2i:%-3i%-5i%-5i", h->levelNum, h->name,h->score, h->time/60, h->time%60, h->moves, h->combos);
    txtWriteCenter(screen, FONTSMALL, buf, HSCREENW,(HSCREENH-30)+(i*12));
    i++;
  }
}

//Return score place (1 = first) or 0 if score place > 10
int statsIsHighScore()
{
  int place=1;
  hsEntry_t* hs;

  if( player()->inEditor )
  {
    return(0);
  }

  listItem* it = &st.packHsTable->begin;
  if( player()->campStats.score < 1000 ) return(0); //No highscores if under 10k
  while( LISTFWD(st.packHsTable,it) )
  {
    hs=(hsEntry_t*)it->data;
    if( player()->campStats.score > hs->score )
    {
      return(place);
    }
    place++;
  }

  if(place>10)
    place=0;

  return(place);
}

//Add to list
void statsSaveHighScore()
{
  int place = statsIsHighScore();
  int p=0;

  listItem* it = &st.packHsTable->begin;
  hsEntry_t* hs;

  //Copy from player struct.
  hs = malloc(sizeof(hsEntry_t));
  memcpy( hs, &(player()->campStats), sizeof(hsEntry_t) );

  listInsertAtIdx(st.packHsTable, (void*)hs, place-1);//-1 since listInsert expects 0 to be the first position, but statsIsHigh returns >0 to be a valid highscore.


  //Trim the list if there are more than 10.
  while( LISTFWD(st.packHsTable,it) )
  {
    p++;
    if(p>10)
    {
      it=listRemoveItem(st.packHsTable,it, LIST_PREV);
    }
  }

  //Save to file.
  statsSave();

  //Clear player.
  initPlayer();

}

void statsReset()
{
  printf("Cleared progress for current pack.\n");
  initPlayer();
  packUnlinkHsFile();
  statsLoad();
}

void packUnlinkHsFile()
{
  //First we find out if it exists on the filesystem
  FILE* f=fopen(st.hsFn, "r");
  if(f)
  {
    fclose(f);
    unlink(st.hsFn);
  }
}

#if defined (PLATFORM_SUPPORTS_STATSUPLOAD)
#include <SDL/SDL_thread.h>
static char curlbuf[2048];
typedef struct thrDat
{ char* cmd;
int* ret;
} thrDat_t;

int upStatsThread(void * d)
{
  char pBuf[2048];
  memset(pBuf, 0, sizeof(pBuf));
  FILE *pipe;
  thrDat_t* dat = (thrDat_t*)d;
  char* cmd = dat->cmd;
  int* ret = dat->ret;

  if( setting()->showWeb ) { printf( "%s\n", cmd ); }

  if( (pipe = popen( cmd, "r" )) != NULL )
  {
      setting()->online=1;

      if( fread( pBuf, 1,2047, pipe)!= 0 )
      {
        if( setting()->showWeb ) { printf("Server returned:'%s'\n",pBuf); }
        if(ret)
        {
          *ret=atoi(pBuf);
        }
      }
      pclose(pipe);

  } else {
    setting()->online=0;
  }
  free(dat->cmd);
  free(dat);
  return(0);
}
#endif

void statsUpload(int level, int time, int moves, int combos, int score, const char* action, int ignoreIfOnline, int* retVal)
{
  #if defined (PLATFORM_SUPPORTS_STATSUPLOAD)
  if( (setting()->online || ignoreIfOnline) )
  {
    int b = sprintf( curlbuf, "%s\"version=%s&pack=%s&level=%i&time=%i&moves=%i&combos=%i&score=%i&action=%s&session=%i&platform=%s\"",\
        CMD_UPLOAD_STATS_POST, VERSION_STRING, packState()->cp->path,\
        level,time,moves,combos,score,action, setting()->session, STR_PLATFORM );
    if(b > 0 && b < 2048)
    {
      thrDat_t* thrData=malloc(sizeof(thrDat_t));
      thrData->cmd=malloc(strlen(curlbuf)+1);
      strcpy(thrData->cmd,curlbuf);
      thrData->ret=retVal;
      if( SDL_CreateThread( upStatsThread, (void*)thrData ) == NULL )
      {
        printf("Warning: Coulnd't start thread: %s\n", SDL_GetError());
      }
    } else {
      printf("Error: sprintf returned %i\n", b);
    }
  }
  #endif
}
