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
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "mbrowse.h"

#ifdef WIN32
  #include <direct.h>
  #define cwd _getcwd
#else
  #include <unistd.h>
  #define cwd getcwd
#endif

static list_t* fl=0;

static void fileListFreeItem(fileListItem_t* i)
{
  free(i->name);
  free(i->fullName);
  free(i);
}

void fileListFree()
{
  listItem* it=&fl->begin;
  if(it)
  {
    while( LISTFWD(fl,it) )
    {
      fileListFreeItem( (fileListItem_t*)(it->data) );
    }
    listFree(fl);
    fl=0;
  }
}

//This little gem is really cool, it resolves a path, relative to the program, to an absolute path
//Unfortunately, after writing it, I discovered it was not what I needed.
/*char* cleanPath(const char* path)
{
  char* fp = cwd(0,0);
  char* buf = malloc(sizeof(char)*2048);
  strcpy(buf, fp);

  int p=0;
  int i,c;


  printf("Full Path to Wizznic executable: %s\n",fp);
  while(path[p])
  {
    if( path[p]=='.' && path[p+1]=='/')
    {
      //p++;
    } else if(path[p]=='.' && path[p+1]=='.')
    {
      p++;
      //Cut off after last /
      i=strlen(buf);
      c=0;
      while(i>-1)
      {
        if(buf[i]=='/')
        {
          buf[i]=0;
          c++;
          if(c==2)
            break;
        }
        i--;
      }
    } else {
      //Copy next char
      i=strlen(buf);
      buf[i]=path[p];
      buf[i+1]=0;
     // p++;

    }
    printf("Buf: '%s'\n", buf);
    free(fp);
    p++;
  }

  free(path);
  path=malloc( sizeof(char)*(strlen(buf)+1));
  strcpy(path,buf);

  free(buf);
  return(path);
}*/


void fileListMake(const char* path)
{

  DIR *pdir=0;
  struct dirent *pent;
  struct stat st;
  char* buf=malloc(sizeof(char)*512);
  fileListItem_t* fi;

  if(fl) fileListFree();
  fl=listInit(NULL);

  //Add ".."
  fi=malloc(sizeof(fileListItem_t));
  fi->name=malloc(sizeof(char)*(strlen("[..]")+1));
  strcpy(fi->name, "[..]");

  fi->fullName=(char*)0;
  fi->dir=1;

  listAppendData(fl,(void*)fi);



  pdir = opendir(path);
  if(!pdir)
    pdir=opendir(".");

  if(pdir)
  {
    while( (pent=readdir(pdir)) )
    {
      //We're not going to read hidden files or . / ..
      if( pent->d_name[0] != '.' )
      {
        sprintf(buf, "%s/%s", path, pent->d_name);

        if(stat(buf, &st)==0)
        {
          fi=malloc(sizeof(fileListItem_t));
          fi->fullName=malloc(sizeof(char)*(strlen(buf)+1));
          strcpy(fi->fullName, buf);

          if((st.st_mode&S_IFDIR)==S_IFDIR )
          {
            fi->dir=1;
            sprintf(buf, "[%s]", pent->d_name);
          } else {
            //Only add ogg or mp3's
            fi->dir=-1;
            if( strcmp( ".mp3", pent->d_name+(strlen(pent->d_name)-4))==0 ||
                strcmp( ".ogg", pent->d_name+(strlen(pent->d_name)-4))==0)
                {
                  fi->dir=0;
                }
            strcpy(buf, pent->d_name);
          }

          fi->name=malloc(sizeof(char)*(strlen(buf)+1));
          strcpy(fi->name, buf);

          if(fi->dir!=-1)
          {
            listAppendData(fl, (void*)fi);
          } else {
            fileListFreeItem(fi);
          }

        }
      }
    }
  }

  closedir(pdir);
  free(buf);
}

list_t* fileList()
{
  return(fl);
}
