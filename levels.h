#ifndef LEVELS_H_INCLUDED
#define LEVELS_H_INCLUDED

#include "list.h"
#include "stats.h"

struct levelInfo_s
{
  char* file;      //Level filename
  char* imgFile;   //Preview filename
  char* author;    //Author name
  char* levelName; //Level name
  char* tileFile;  //tileFile
  char* bgFile;    //backgroundFile
  char* musicFile; //Music file
  char* soundDir;  //Directory containing soundeffects
  char* fontName;  //Font
  char* cursorName;//Cursor
  char* startImg; //img to show before game starts
  char* stopImg;  //img to show after level completed.

  listItem* teleList; //Initiated by mkLevelInfo

  hsEntry_t* hsStats; //hsEntry level stats. (TODO: Should this be freed by freeLevelInfo)

  int time; //Time in seconds to complete
};
typedef struct levelInfo_s levelInfo_t;

void makeLevelList(listItem** list, const char* dir);
void makeUserLevelList();
void addUserLevel(const char* fn);
levelInfo_t* mkLevelInfo(const char* fileName);
void freeLevelInfo(levelInfo_t** p); //given a pointer to the pointer, so it can dereference it properly.
char* userLevelFile(int num);
int getNumUserLevels();

#endif // LEVELS_H_INCLUDED
