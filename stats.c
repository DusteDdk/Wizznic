#include "strings.h"
#include "stats.h"
#include "pack.h"
#include "defs.h"
#include "player.h"
#include "list.h"
#include "settings.h"
#include "userfiles.h"

static stats_t st;

//Return ptr to stats_t
inline stats_t* stats()
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
  listItem* it;
  hsEntry_t* hs;
  hsEntry_t ths;
  statsFileHeader_t sfh;

  //Set progress -1 before reading the real progress from file (in case there is no file yet)
  // -1 because progress is updated to current-level after the completion of that level, but
  // at the same time, it is used (with +2) to set maxY in menu.
  st.progress=-1;

  //Free levelStats if allready filled
  if(st.levelStats)
  {
    it = st.levelStats;
    while( (it = it->next) )
    {
      free(it->data);
    }
    freeList( st.levelStats );
  }

  //Create new list for levelStats
  st.levelStats = initList();
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
    listAddData(st.levelStats, (void*)hs);
  }

  //Remove packwide highscores if they exist
  if(st.packHsTable)
  {
    it=st.packHsTable;
    while( (it=it->next) )
    {
      free(it->data);
    }
    freeList(st.packHsTable);
  }

  //Create new list for packWide highscores
  st.packHsTable = initList();


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
    fread( &i, sizeof(int),1,f);
    if(i == STATS_FILE_FORMAT_VERSION )
    {
      //Jump to start of file again
      rewind(f);
      //Read the header
      fread( (void*)(&sfh), sizeof(statsFileHeader_t), 1, f);
      //Set progress
      st.progress = sfh.progress;

      //Override defaults for levels found in file
      for(i=0; i < sfh.numLevelEntries; i++)
      {
        fread( (void*)(&ths), sizeof(hsEntry_t), 1, f );
        //Find the list to put it in
        hs = (hsEntry_t*)listGetItemData(st.levelStats,ths.levelNum);
        //If it was found, copy it
        if(hs)
        {
          memcpy( hs, &ths, sizeof(hsEntry_t) );
        }
      }
      //Fill in the pack-wide highscore list
      for(i=0; i < sfh.numHsEntries; i++)
      {
        hs=malloc(sizeof(hsEntry_t));
        fread(hs,sizeof(hsEntry_t),1,f);
        listAddData(st.packHsTable, (void*)hs);
      }
    } else {
      printf("File '%s' is version %i but current version is %i, delete the file.\n", st.hsFn, i, STATS_FILE_FORMAT_VERSION);
    }

    fclose(f);
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
    sfh.numLevelEntries = listSize( st.levelStats );
    sfh.numHsEntries  = listSize( st.packHsTable );

    //Write header
    fwrite( (void*)(&sfh), sizeof(statsFileHeader_t),1, f );

    //Write levelstats
    it=st.levelStats;
    while( (it = it->next) )
    {
      hs=(hsEntry_t*)it->data;
      //Write entry to file.
      fwrite( (void*)(hs), sizeof(hsEntry_t), 1, f);
    }

    //Write packWide hstable
    it = st.packHsTable;
    while( (it = it->next) )
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

  statsUpload(player()->level, lvlHs->time, lvlHs->moves,lvlHs->combos,lvlHs->score, "complete",0);

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
  st.cl=listGetItemData(st.levelStats, l);
}

void statsDrawHs(SDL_Surface* screen)
{
  listItem* it = stats()->packHsTable;
  hsEntry_t* h;
  int i=0;
  char buf[64];

  txtWriteCenter(screen, FONTMEDIUM,"Highscores!", HSCREENW, HSCREENH-105);

  txtWriteCenter(screen, FONTSMALL, packState()->cp->name, HSCREENW, HSCREENH-80);

  txtWriteCenter(screen, FONTSMALL, "L Name      Score  Time  Move Combo", HSCREENW,HSCREENH-50);
  txtWriteCenter(screen, FONTSMALL, "+-+---------+------+-----+----+----", HSCREENW,HSCREENH-40);

  while( (it=it->next) )
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
  listItem* it = st.packHsTable;
  if( player()->campStats.score < 1000 ) return(0); //No highscores if under 10k
  while( (it=it->next) )
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

  listItem* it = st.packHsTable;
  hsEntry_t* hs;

  //Copy from player struct.
  hs = malloc(sizeof(hsEntry_t));
  memcpy( hs, &(player()->campStats), sizeof(hsEntry_t) );

  listInsertData(st.packHsTable, (void*)hs, place-1); //-1 since listInsert expects 0 to be the first position, but statsIsHigh returns >0 to be a valid highscore.

  //Trim the list if there are more than 10.
  while( (it=it->next) )
  {
    p++;
    if(p>10)
    {
      it=listRemoveItem(st.packHsTable,it);
    }
  }

  //Save to file.
  statsSave();

  //Clear player.
  initPlayer();

}

#ifndef GP2X
#include <SDL/SDL_thread.h>
static int threadRunning=0;
static char curlbuf[2048];
int upStatsThread(void * d)
{
  if( system( (char*)d )==0 )
  {
    if(!setting()->online)
    {
      setting()->online=1;
    }
  } else {
    printf("Thread: system('%s') Failed.\n",(char*)d);
    setting()->online=0;
  }
  threadRunning=0;
  return(0);
}
#endif

void statsUpload(int level, int time, int moves, int combos, int score, const char* action, int ignoreIfOnline)
{
  #ifndef GP2X
  if( !threadRunning && (setting()->online || ignoreIfOnline) )
  {
    threadRunning=1; //Thread will set this 0 when returning.

    int b = sprintf( curlbuf, "%s\"version=%s&pack=%s&level=%i&time=%i&moves=%i&combos=%i&score=%i&action=%s\"",\
     CURLBIN, VERSION_STRING, packState()->cp->path,\
    level,time,moves,combos,score,action );
    if(b > 0 && b < 2048)
    {
      if( SDL_CreateThread( upStatsThread, (void*)curlbuf ) == NULL )
      {
        printf("Warning: Coulnd't start thread: %s\n", SDL_GetError());
      }
    } else {
      printf("Errir; printf returned %i\n", b);
    }
  }
  #endif
}
