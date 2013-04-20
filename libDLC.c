/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2013 Jimmy Christensen <dusted@dusted.dk>             *
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

#include "defs.h"


#include <unistd.h>
#include <string.h>
#include <SDL/SDL.h>

#include "userfiles.h"
#include "libDLC.h"
#include "bundle.h"

#define DLC_API_VERSION_STRING "Wizznic 0x10"

typedef struct {
    char* requestCmd;
    char* destFileName;
} dlcThreadCmd;

static int dlcState=DLC_BUSY;

int dlcDownloadThread(void * d)
{
  dlcState=DLC_BUSY;
  dlcThreadCmd* cmd = (dlcThreadCmd*)d;

  if( system( cmd->requestCmd ) == 0)
  {
    printf("Bundle download success.\n");
    int ret = debundle( cmd->destFileName, getUsrPackDir() );
    if( ret == BUNDLE_SUCCESS  )
    {
      printf("Bundle installation success.\n");
      dlcState = DLC_INSTALLED;
    } else {
      dlcState = DLC_BUNDLE_ERROR;

      unlink("Bundle installation failed.\n");
      printf("Extraction failed.\n");
    }

    unlink(cmd->destFileName);
  } else  {
    printf("Bundle download failed.\n");
  }

  if( dlcState == DLC_BUSY )
  {
    dlcState=DLC_FAILED;
  }

  free( cmd->destFileName );
  free( cmd->requestCmd );
  free( cmd );

  return(0);
}

void dlcTryInstall( const char* code, const char* dest )
{
  dlcThreadCmd* cmd = malloc( sizeof(dlcThreadCmd) );
  printf("Trying to install dlc %s into %s\n", code, dest);

  cmd->destFileName = malloc( strlen(getUsrPackDir())+strlen(code)+6 ); // 1 for / 4 for .bin and 1 for null terminator.
  cmd->requestCmd = malloc( 2048+strlen(cmd->destFileName) );

  sprintf( cmd->destFileName, "%s/%s.bin", getUsrPackDir(), code );
  sprintf( cmd->requestCmd, "%s -O %s %sget/%s", DLC_PROGRAM,cmd->destFileName, DLC_SERVER_URL, code);

  if( SDL_CreateThread( dlcDownloadThread, cmd ) == NULL )
  {
    printf("Error: Coulnd't start dlc download thread: %s\n", SDL_GetError());
  }
  //Do not free cmd, the thread will do that.
}

int dlcCheckOnlineThread(void * d)
{
  dlcState=DLC_BUSY;

  char* pBuf = malloc( 128 );
  FILE *pipe;
  dlcThreadCmd* cmd = (dlcThreadCmd*)d;

  if( (pipe = popen( cmd->requestCmd, "r" )) != NULL )
  {
    if( fgets( pBuf, 127, pipe)!= NULL )
    {
      if( pBuf[0] == 'W' && strlen(pBuf)>=strlen(DLC_API_VERSION_STRING))
      {
        //Truncate the string
        pBuf[strlen(DLC_API_VERSION_STRING)]=0;

        if( strcmp(pBuf, DLC_API_VERSION_STRING) == 0 )
        {
          dlcState = DLC_READY;
        } else {
          dlcState = DLC_API_OUTDATED;
        }
      }
    }

    fclose(pipe);
  }

  free(pBuf);
  return(0);
}

void dlcCheckOnline()
{
  dlcThreadCmd* cmd = malloc( sizeof(dlcThreadCmd) );

  cmd->destFileName = NULL;
  cmd->requestCmd = malloc( 2048 );

  sprintf( cmd->requestCmd, "%s -O - %sget/cmd_version", DLC_PROGRAM, DLC_SERVER_URL);

  SDL_CreateThread( dlcCheckOnlineThread, cmd );
  //Do not free cmd, the thread will do that.
}

int dlcGetState()
{
  return(dlcState);
}

void dlcSetReady()
{
  dlcState = DLC_READY;
}
