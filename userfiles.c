#include "userfiles.h"


#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char* strConfDir;
static char* strEditLvlDir;
static char* strHsDir;

void initUserPaths()
{
  #ifdef PER_USER_FILES

    //Generate directory string
    if(getenv("XDG_CONFIG_HOME") != NULL)
    {
      strConfDir = malloc( sizeof(char)*(strlen(getenv("XDG_CONFIG_HOME"))+strlen("/wizznic"))+1 );
      sprintf( strConfDir, "%s/wizznic", getenv("XDG_CONFIG_HOME") );
    } else if( getenv("HOME") != NULL)
    {
      strConfDir = malloc( sizeof(char)*(strlen(getenv("HOME"))+strlen("/.wizznic"))+1 );
      sprintf( strConfDir, "%s/.wizznic", getenv("HOME") );
    } else {
      strConfDir = malloc(sizeof(char)*strlen(".")+1);
      sprintf( strConfDir, "." );
    }


  #else
    strConfDir = malloc(sizeof(char)*strlen(".")+1);
    sprintf( strConfDir, "." );
  #endif

  strEditLvlDir = malloc( sizeof(char)*( strlen(strConfDir)+strlen("/editorlevels")+1 ) );
  sprintf( strEditLvlDir, "%s/editorlevels", strConfDir );

  strHsDir = malloc( sizeof(char)*( strlen(strConfDir)+strlen("/highscores")+1 ) );
  sprintf( strHsDir, "%s/highscores", strConfDir );

  //Check if dir exists (we check each dir, since ./ might exist without the others
  struct stat s;
  if (stat(strConfDir,&s) != 0) mkdir(strConfDir,S_IRWXU);
  if (stat(strEditLvlDir,&s) != 0) mkdir(strEditLvlDir,S_IRWXU);
  if (stat(strHsDir,&s) != 0) mkdir(strHsDir,S_IRWXU);

}

char* getConfigDir() { return( strConfDir ); }
char* getHighscoreDir() { return( strHsDir ); }
char* getUserLevelDir() { return( strEditLvlDir ); }
