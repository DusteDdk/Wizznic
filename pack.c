/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2012 Jimmy Christensen <dusted@dusted.dk>             *
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

#include "pack.h"
#include "strings.h"
#include "defs.h"

static packStateType ps;

packStateType* packState() { return(&ps); }

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


void packAdd(const char* packDir)
{
  char* buf = malloc(sizeof(char)*2048);
  char* buf2= malloc(sizeof(char)*1024);
  char* val = malloc(sizeof(char)*1024);
  char* set = malloc(sizeof(char)*1024);

  //This block is for playlists
  listItem* playList=0;
  listItem* li=0;
  playListItem* pli=0;
  int i; //Counter

  FILE* f=0;
  packInfoType* ti = malloc(sizeof(packInfoType));

  printf("Adding pack '%s'\n",packDir);

  //Any levels? (Packs are invalid without a levels folder and atleast one level)
  sprintf(buf, "%s/levels/level000.wzp", packDir);
  if( !isFile(buf) )
  {
    printf("   Error: must contain level000.wzp\n");
    free(ti);

    free(buf);
    free(buf2);
    free(val);
    free(set);

    return;
  }

  //Initialize list for playlist
  playList = initList();

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
              listAddData( playList, (void*)pli );
          } else {
            printf("   Playlist entry format is mus=XX-XX,song name.ogg where XX-XX is a level range.\n");
          }
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
//    printf("   Warning: '%s' not found.\n", buf);
    ti->icon = loadImg( DATADIR"data/noicon.png" );
  }

  //Check if pack have a "finished" icon.
  sprintf(buf, "%s/finished.png", packDir);
  if( !isFile( buf ) )
  {
    ti->hasFinishedImg=0;
//    printf("   Warning: '%s' not found.\n", buf);
  } else {
    ti->hasFinishedImg=1;
  }

  //Set ps.cp before entering makeLevelist, it makes use of packGetFile
  ps.cp = ti;

  //Add levels.
  ti->levels=0;
  makeLevelList( &ti->levels, packDir); //makeLevelList looks in packDir/levels/

  //update num+
  ti->numLevels = listSize( ti->levels ) -1 ; //The last level does not count

  //Add to list of packages
  listAddData( ps.packs, (void*)ti );

  //Increase number of available packages
  ps.numPacks++;

  //Put playlist songs into levelfiles.
  li=playList;
  while( (li=li->next) )
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
  li=playList;
  while( (li=li->next) )
  {
    pli=(playListItem*)li->data;
    free( pli->song );
    free(li->data);
  }
  freeList( playList );
  playList=0;

  free(buf);
  free(buf2);
  free(val);
  free(set);
}

int strCmp(const void* a, const void* b)
{
  return( strcmp( *(char**)a, *(char**)b) );
}

void packInit()
{
  DIR *pdir=0;
  struct dirent *pent;
  struct stat st;
  char* buf=malloc(sizeof(char)*128);

  printf("initPack();\n");

  ps.packs = initList();
  ps.numPacks=0;

  //Add the wizznic pack as nr 0.
  packAdd( DATADIR"packs/000_wizznic" );

  pdir = opendir( DATADIR"packs/" );

  listItem* packDirList = initList();

  if(pdir)
  {
    while( (pent=readdir(pdir)) )
    {
      //We're not going to read hidden files or . / ..
      if(pent->d_name[0] != '.')
      {
        sprintf(buf, DATADIR"packs/%s",pent->d_name);
        if(stat(buf, &st)==0)
        {
          if( (st.st_mode&S_IFDIR)==S_IFDIR )
          {
            //Ignore the "wizznic" directory since it's allready added.
            if(strcmp( buf, DATADIR"packs/000_wizznic" ) != 0)
            {
              //packAdd(buf);
              char* pdstr = malloc( sizeof(char)*strlen(buf)+1 );
              strcpy( pdstr, buf );
              listAddData( packDirList, (void*)pdstr );
            }
          }
        } else {
          printf("packInit(); Error: stat('%s') failed!\n", buf);
        }
      }
    }
    closedir(pdir);
  } else {
    printf("packInit(); Eror: No packs found.\n");
  }

  //Add the rest of the packs.
  char** sortme = malloc( sizeof(char**) * listSize(packDirList) );
  listItem* it = packDirList;
  int i=0;
  while( (it = it->next) )
  {
    sortme[i]=(char*)it->data;
    i++;
  }

  //Sort the array
  qsort( sortme, listSize(packDirList), sizeof(char*), strCmp );

  //Loop through array, add pack, free string
  for(i=0; i<listSize(packDirList); i++)
  {
    //add pack
    packAdd( sortme[i] );
    //free string
    free( sortme[i] );
  }

  //Free sortme array
  free(sortme);

  //Free packDirList
  freeList( packDirList );

  free(buf);
  buf=0;

  printf("initPack(); Added %i packs.\n", listSize(ps.packs));

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
    sprintf( buf, "%s/%s/%s", ((packInfoType*)listGetItemData(ps.packs,0))->path, path , fn );
  }

  return(buf);
}


levelInfo_t* levelInfo(int num)
{
  return( (levelInfo_t*)listGetItemData(ps.cp->levels,num) );
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

  ps.cp = (packInfoType*)listGetItemData(ps.packs,ps.selected);

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
  int i;
  for(i=0; i < ps.numPacks; i++)
  {
    if( strcmp( ((packInfoType*)listGetItemData(ps.packs, i))->path , dir)==0 )
    {
      packSet(i);
      return;
    }
  }
  packSet(0);
  return;
}


void packFreeGfx()
{
  SDL_FreeSurface(ps.packBoxImg);
  free(ps.packBoxSpr[0]);
  free(ps.packBoxSpr[1]);
  ps.packBoxImg=0;
  ps.packBoxSpr[0]=0;
  ps.packBoxSpr[1]=0;
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
    ps.packBoxSpr[0] = cutSprite(ps.packBoxImg, 0,0,260,42);
    ps.packBoxSpr[1] = cutSprite(ps.packBoxImg, 0,42,260,42);
  }

  //PackInfo is now in pi.
  packInfoType* pi = (packInfoType*)listGetItemData(ps.packs,packNum);

  //Blit the box
  if(ps.selected==packNum)
    drawSprite(screen, ps.packBoxSpr[0], posx, posy);
  else
    drawSprite(screen, ps.packBoxSpr[1], posx, posy);

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

  //Comment

}
