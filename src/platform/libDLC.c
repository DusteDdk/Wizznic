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

#include "libDLC.h"

#include <unistd.h>
#include <string.h>
#include <SDL/SDL.h>

#include "../defs.h"
#include "../settings.h"
#include "../userfiles.h"
#include "../bundle.h"

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

  if( setting()->showWeb ) { printf( "%s\n", cmd->requestCmd ); }

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
  sprintf( cmd->requestCmd,CMD_DOWNLOAD_DLC_FILE, code, cmd->destFileName);

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

  if( setting()->showWeb ) { printf( "%s\n", cmd->requestCmd ); }

  if( (pipe = popen( cmd->requestCmd, "r" )) != NULL )
  {
    if( fgets( pBuf, 127, pipe)!= NULL )
    {
      dlcState = DLC_API_OUTDATED;
      if( pBuf[0] == 'W' && strlen(pBuf)>=strlen(DLC_API_VERSION_STRING))
      {
        //Truncate the string
        pBuf[strlen(DLC_API_VERSION_STRING)]=0;

        if( strcmp(pBuf, DLC_API_VERSION_STRING) == 0 )
        {
          dlcState = DLC_READY;
        }
      }
    }

    fclose(pipe);
  }

  free(pBuf);
  free( cmd->requestCmd );
  free( cmd );

  return(0);
}

void dlcCheckOnline()
{
  dlcThreadCmd* cmd = malloc( sizeof(dlcThreadCmd) );

  cmd->destFileName = NULL;
  cmd->requestCmd = malloc( 2048 );

  sprintf( cmd->requestCmd, CMD_CHECK_DLC_API_VERSION);

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

#ifdef PLATFORM_SUPPORTS_STATSUPLOAD
int dlcPushFile( const char* fileName, char* msg )
{
  int ret=0;
  FILE *pipe;
  char upCmd[4096];
  char pBuf[128];
  sprintf( upCmd, CMD_UPLOAD_DLC_FILE, fileName );

  if( setting()->showWeb ) { printf( "%s\n", upCmd ); }

  msg[0]=0;
  if( (pipe = popen( upCmd, "r" )) != NULL )
  {
    if( fgets( pBuf, 127, pipe)!= NULL )
    {
      strcpy(msg, pBuf);
    }

    fclose(pipe);
  }

  //We got the "levelX" code back, alls good.
  if(msg[0]=='l')
  {
    ret=1;
  }

  return(ret);
}
#endif
