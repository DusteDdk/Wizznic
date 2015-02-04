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

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "settings.h"
#include "pack.h"
#include "strings.h"
#include "defs.h"
#include "userfiles.h"
#include "bundle.h"
#include "platform/libDLC.h"

#define PCKLISTIMG_SELECTED 0
#define PCKLISTIMG_DEFAULT  1
#define PCKLISTIMG_IS_DLC 2
#define PCKLISTIMG_DLC_ENTER 3
#define PCKLISTIMG_DLC_OFFLINE 4


static packStateType ps;

packStateType* packState() { return(&ps); }

void _freePlaylistItem(void* data)
{
  playListItem* pli=(playListItem*)data;
  free( pli->song );
  free(data);
}

int isFile(const char* fileName)
{
  struct stat st;
  if(stat(fileName, &st)==0)
  {
    if( (st.st_mode&S_IFREG) == S_IFREG )
    {
      return(1);
    }
  }
  return(0);
}

int isDir(const char* dirName)
{
  struct stat st;
  if(stat(dirName, &st)==0)
  {
    if( (st.st_mode&S_IFDIR) == S_IFDIR )
    {
      return(1);
    }
  }
  return(0);
}


int packAdd(const char* packDir, int isDLC)
{
  char* buf = malloc(sizeof(char)*2048);
  char* buf2= malloc(sizeof(char)*1024);
  char* val = malloc(sizeof(char)*1024);
  char* set = malloc(sizeof(char)*1024);

  //This block is for playlists
  list_t* playList=0;
  listItem* li=0;
  playListItem* pli=0;
  int i; //Counter

  FILE* f=0;
  packInfoType* ti = malloc(sizeof(packInfoType));
  ti->lives=3; //Default 3 lives, if pack do not define another number.
  ti->isDLC=isDLC;

  //Any levels? (Packs are invalid without a levels folder and atleast one level)
  sprintf(buf, "%s/levels/level000.wzp", packDir);
  if( !isFile(buf) )
  {
    printf("Error: Pack '%s' must contain level000.wzp\n", packDir);
    free(ti);

    free(buf);
    free(buf2);
    free(val);
    free(set);

    return(0);
  }

  //Initialize list for playlist
  playList = listInit(_freePlaylistItem);

  //Open packs/packname/info.ini
  sprintf(buf, "%s/info.ini", packDir);
  f = fopen(buf, "r");
  if(f)
  {
    while( fgets(buf, 128, f) )
    {
      stripNewLine(buf);
      if(splitVals('=',buf,set,val))
      {
        if(strcmp("author", set)==0)
        {
          ti->author = malloc( sizeof(char)*(strlen(val)+1+3) );
          sprintf(ti->author, "By %s", val);
        } else
        if(strcmp("packname", set)==0)
        {
          ti->name = malloc( sizeof(char)*(strlen(val)+1) );
          strcpy(ti->name,val);
        } else
        if(strcmp("comment", set)==0)
        {
          ti->comment = malloc( sizeof(char)*(strlen(val)+1+1) );
          sprintf(ti->comment, "-%s", val);
        } else
        if(strcmp("mus", set)==0)
        {
          //mus=00,song
          if( splitVals(',',val, buf,set ) && splitVals('-',buf,val,buf2) )
          {
              //val= from buf2=to set=song name
              pli = malloc( sizeof( playListItem ) );
              pli->from=atoi(val);
              pli->to=atoi(buf2);
              pli->song=malloc(sizeof(char)*strlen(set)+1);
              strcpy( pli->song, set );
              listAppendData( playList, (void*)pli );
          } else {
            printf("   Playlist entry format is mus=XX-XX,song name.ogg where XX-XX is a level range.\n");
          }
        } else
        if(strcmp("lives", set)==0)
        {
          ti->lives = atoi(val);
        }
      } //Found =
    } //reading file

    //Close the info file.
    fclose(f);
  } else {
    //Fall back if no file was found.
    printf("   Warning: '%s' not found, using defaults.\n",buf);
    ti->author = malloc( sizeof(char)* 20 );
    strcpy(ti->author, "info.ini not found");
    ti->comment=ti->author;
    ti->name = malloc( sizeof(char)*(strlen(packDir)+1) );
    strcpy(ti->name, packDir);
  }


  //Set path
  ti->path = malloc( sizeof(char)*(strlen(packDir)+1) );
  strcpy(ti->path,packDir);

  //Set pack icon
  sprintf(buf, "%s/icon.png", packDir);
  ti->icon = loadImg(buf);
  if(!ti->icon)
  {
    ti->icon = loadImg( DATADIR"data/noicon.png" );
  }

  //Check if pack have a "finished" icon.
  sprintf(buf, "%s/finished.png", packDir);
  if( !isFile( buf ) )
  {
    ti->hasFinishedImg=0;
  } else {
    ti->hasFinishedImg=1;
  }

  //Set ps.cp before entering makeLevelist, it makes use of packGetFile
  ps.cp = ti;

  //Add levels.
  ti->levels=0;
  ti->levels=makeLevelList(packDir); //makeLevelList looks in packDir/levels/

  //set number of levels in pack
  ti->numLevels = ti->levels->count - 1  ; //The last level does not count (because it is just a "completed" screen).


  //Add to list of packages
  listAppendData( ps.packs, (void*)ti );

  //Increase number of available packages
  ps.numPacks++;

  //Put playlist songs into levelfiles.
  li=&playList->begin;
  while( LISTFWD(playList,li) )
  {
    pli=(playListItem*)li->data;

    for(i=0; i<ti->numLevels;i++)
    {
      if(i >= pli->from && i <= pli->to)
      {
        levelInfo(i)->musicFile = malloc( sizeof(char)*strlen(pli->song)+1 );
        strcpy(levelInfo(i)->musicFile, pli->song);
      }
    }
  }

  //Clear playlist data
  listFree( playList );

  free(buf);
  free(buf2);
  free(val);
  free(set);

  return(ps.numPacks-1);
}



int strCmp(const void* a, const void* b)
{
  return( strcmp( *(char**)a, *(char**)b) );
}

void packScanDir( const char* path, list_t* dirList )
{
  struct dirent *pent;
  struct stat st;
  char* buf=malloc(sizeof(char)*2048);
  DIR *pdir= opendir( path );

  if(pdir)
  {
    while( (pent=readdir(pdir)) )
    {
      //We're not going to read hidden files or . / ..
      if(pent->d_name[0] != '.')
      {
        sprintf(buf, "%s/%s",path,pent->d_name);
        if(stat(buf, &st)==0)
        {
          if( (st.st_mode&S_IFDIR)==S_IFDIR )
          {
            //Ignore the "wizznic" directory since it's allready added.
            if(strcmp( buf, DATADIR"packs/000_wizznic" ) != 0)
            {
              char* pdstr = malloc( sizeof(char)*strlen(buf)+1 );
              strcpy( pdstr, buf );
              listAppendData( dirList, (void*)pdstr );
            }
          } else if(  (st.st_mode&S_IFREG) )
          {
            //It's a file, let's try and see if it's a bundle.
            int l = strlen(buf);
            if( l > 4 && strcasecmp( &buf[l-4] ,".wiz" )==0 )
            {

              l = debundle( buf, getUsrPackDir() );

              if( l == BUNDLE_SUCCESS )
              {
                printf("Installed bundle '%s'.\n", buf);
                char* pdstr = malloc( sizeof(char)*strlen(buf)+1 );
                strcpy( pdstr, bundlePath() );
                listAppendData( dirList, (void*)pdstr );
                unlink( buf );
              } else if( l == BUNDLE_FAIL_CORRUPT )
              {
                printf("The bundle file '%s' is corrupt, deleting it.\n", buf);
                unlink( buf );
              } else if( l == BUNDLE_FAIL_UNSUPPORTED_VERSION )
              {
                printf("The bundle file '%s' is not supported by this version of Wizznic, please try and update Wizznic.\n", buf);
              } else if( l == BUNDLE_FAIL_DIR_EXISTS )
              {
                printf("The bundle file '%s' has already been installed.\n", buf);
              }

              bundlePathReset();
            }

          }
        } else {
          printf("packScanDir(); Error: stat('%s') failed!\n", buf);
        }
      }
    }
    closedir(pdir);
  }

  free(buf);
}

void packInit()
{

  printf("initPack();\n");

  ps.packs = listInit(NULL);
  ps.numPacks=0;

  //Add the wizznic pack as nr 0.
  packAdd( DATADIR"packs/000_wizznic", 0 );

  list_t* packDirList = listInit(NULL);
  list_t* usrPackDirList = listInit(NULL);

  packScanDir( DATADIR"packs", packDirList);
  packScanDir( getUsrPackDir(), usrPackDirList);

  if(packDirList->count < 1)
  {
    printf("packInit(); Error: No packs found.\n");
  }

  //Add pack directories to list of strings to be sorted
  char** sortme = malloc( sizeof(char**) * packDirList->count );
  int i=0;
  listItem* it = &packDirList->begin;
  while( LISTFWD(packDirList,it) )
  {
    sortme[i]=(char*)it->data;
    i++;
  }

  //Sort the array
  qsort( sortme, packDirList->count, sizeof(char*), strCmp );

  //Loop through array, add pack, free string
  for(i=0; i<packDirList->count; i++)
  {
    //add pack
    packAdd( sortme[i], PACK_IS_NOT_DLC );
    //free string
    free( sortme[i] );
  }

  //Append the unsorted list of DLC packs.
  it=&usrPackDirList->begin;
  while( LISTFWD(usrPackDirList,it) )
  {
    packAdd( (char*)it->data, PACK_IS_DLC );
  }

  //Free sortme array
  free(sortme);

  //Free packDirList
  //FIXME: Check if the data or pointers are copied, maybe we could free ?
  listFree( packDirList );

  printf("initPack(); Added %i packs.\n", ps.packs->count);

  //Do not call packSet here, it will be called after setting's are read.

  //Set the packbox graphics ptr 0 before loading it.
  ps.packBoxImg=0;
}

const char* packGetFile(const char* path,const char* fn)
{
  //First, see if file exists in selected pack.
  static char buf[4096];
  sprintf( buf, "%s/%s/%s", ps.cp->path, path, fn );
 // printf("packGetFile(): request for '%s'\n", buf);

  //Can't find the file in the pack? default to pack0 (wizznic standard pack)
  if( !isFile(buf) )
  {
    sprintf( buf, "%s/%s/%s", ((packInfoType*)(ps.packs->begin.next->data))->path, path , fn );
  }

  return(buf);
}


levelInfo_t* levelInfo(int num)
{
  levelInfo_t* li = (levelInfo_t*)(listGetItemAt(ps.cp->levels,num)->data);
  return( li );
}

int getNumLevels()
{
  return( ps.cp->numLevels );
}

//Select packNum (or default if out of range
void packSet(int packNum)
{
  if(packNum < ps.numPacks)
    ps.selected=packNum;
  else
  {
    printf("packSet(); Error: packNum '%i' out of range (%i)\n",packNum, ps.numPacks);
    ps.selected=0;
  }

  printf("packSet(); Selecting pack %i...\n", ps.selected);
  ps.cp = (packInfoType*)listGetItemAt(ps.packs, ps.selected)->data;

  //Set finishedImg 0 when we select a pack, to make sure the correct image is loaded.
  ps.finishedImg=0;

  printf("packSet(); Selected pack '%s' Loading stats..\n", ps.cp->path);
  //Load stats for this pack
  statsLoad();
  printf("packSet(); Stats loaded.\n");

}

//Search through packs to find dir, if none found, default to 0.
//This algorithm is a lot slower than doing the while( (it=it->next) ) however
//it is also smaller and only used once.
void packSetByPath(const char* dir)
{
  int i=0;
  listItem* it=&ps.packs->begin;
  while( LISTFWD(ps.packs,it) )
  {
    if( strcmp( ((packInfoType*)it->data)->path,dir ) == 0 )
    {
      packSet(i);
      return;
    }
    i++;
  }

  printf( "packSetByPath(); Error: Could not find pack with path '%s'\n",dir);

  packSet(0);
  return;
}


void packFreeGfx()
{
  SDL_FreeSurface(ps.packBoxImg);
  free(ps.packBoxSpr[0]);
  free(ps.packBoxSpr[1]);
  free(ps.packBoxSpr[2]);
  free(ps.packBoxSpr[3]);
  free(ps.packBoxSpr[4]);
  ps.packBoxImg=0;
  ps.packBoxSpr[0]=0;
  ps.packBoxSpr[1]=0;
  ps.packBoxSpr[2]=0;
  ps.packBoxSpr[3]=0;
  ps.packBoxSpr[4]=0;
}

void drawPackBox(SDL_Surface* screen,int posx, int posy,int packNum)
{
  char buf[128];
  SDL_Rect r;
  r.x=posx+5;
  r.y=posy+5;

  //We do this so the menu can unload the images when not used.
  if(!ps.packBoxImg)
  {
    //Load graphics
    ps.packBoxImg = loadImg( DATADIR"data/pack-box-small.png" );
    ps.packBoxSpr[PCKLISTIMG_SELECTED] = cutSprite(ps.packBoxImg, 0,0,260,42);
    ps.packBoxSpr[PCKLISTIMG_DEFAULT] = cutSprite(ps.packBoxImg, 0,42,260,42);
    ps.packBoxSpr[PCKLISTIMG_IS_DLC] = cutSprite(ps.packBoxImg, 0,42+42,260,42);
    ps.packBoxSpr[PCKLISTIMG_DLC_ENTER] = cutSprite(ps.packBoxImg, 0,42+42+42,260,42);
    ps.packBoxSpr[PCKLISTIMG_DLC_OFFLINE] = cutSprite(ps.packBoxImg, 0,42+42+42+42,260,42);
  }

  //PackInfo is now in pi.
  packInfoType* pi;

  if( packNum == ps.numPacks )
  {
    pi = ps.dlc;
  } else {
    pi = (packInfoType*)( listGetItemAt(ps.packs,packNum) )->data;
  }

  //Blit the box
  if(pi == ps.dlc )
  {

#if defined( PLATFORM_SUPPORTS_STATSUPLOAD )
    if( setting()->online && dlcGetState()==DLC_READY )
    {
      drawSprite(screen, ps.packBoxSpr[PCKLISTIMG_DLC_ENTER], posx, posy);
      txtWrite(screen, FONTSMALL, STR_MENU_PACKLIST_DLC_ENTER, posx+40, posy+4);
    } else {
      drawSprite(screen, ps.packBoxSpr[PCKLISTIMG_DLC_OFFLINE], posx, posy);
      txtWrite(screen, FONTSMALL, STR_MENU_PACKLIST_DLC_OFFLINE, posx+12, posy+4);
    }
#else
    drawSprite(screen, ps.packBoxSpr[PCKLISTIMG_DLC_OFFLINE], posx, posy);
    txtWrite(screen, FONTSMALL, STR_MENU_PACKLIST_DLC_OFFLINE, posx+12, posy+4);
#endif
    return; //We don't want to write any info on that special box.
  } else if(ps.selected==packNum)
  {
    drawSprite(screen, ps.packBoxSpr[PCKLISTIMG_SELECTED], posx, posy);
  } else {
    if(pi->isDLC == PACK_IS_DLC)
    {
      drawSprite(screen, ps.packBoxSpr[PCKLISTIMG_IS_DLC], posx, posy);
    } else {
      drawSprite(screen, ps.packBoxSpr[PCKLISTIMG_DEFAULT], posx, posy);
    }
  }

  //Blit the icon image
  SDL_BlitSurface(pi->icon,0,screen, &r);

  //PackName
  txtWrite(screen, FONTSMALL, pi->name, posx+40, posy+4);

  //Author
  txtWrite(screen, FONTSMALL, pi->author, posx+258-9*(strlen(pi->author)), posy+4);

  //Comment
  txtWrite(screen, FONTSMALL, pi->comment, posx+40, posy+4+12);

  //Number of levels
  sprintf(buf, "%i levels", pi->numLevels);
  txtWrite(screen, FONTSMALL, buf, posx+258-9*(strlen(buf)), posy+4+24);

  //Number of lives
  if( pi->lives > 0 )
  {
    sprintf(buf, "%i lives", pi->lives);
  } else {
    sprintf(buf, "Infinite lives!");
  }
  txtWrite(screen, FONTSMALL, buf, posx+40, posy+4+24);
}
