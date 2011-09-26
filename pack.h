#ifndef PACK_H_INCLUDED
#define PACK_H_INCLUDED

#include <SDL/SDL.h>

#include "list.h"
#include "sprite.h"
#include "levels.h"

struct packInfo_s {
  char* name; //Name user sees
  char* author;
  char* comment;
  char* path; //Path to pack
  SDL_Surface* icon;
  listItem* levels; //LevelInfo*'s
  int numLevels;
  int hasFinishedImg;
};
typedef struct packInfo_s packInfoType;

struct packState_s {
  int numPacks; //Number of packs
  int selected; //Currently selected
  listItem* packs; //PackInfoType*s
  packInfoType* cp; //Current pack
  SDL_Surface* packBoxImg; //Graphics for the box
  SDL_Surface* finishedImg;; //Image shown when cp is completed (0 = not been loaded)
  spriteType* packBoxSpr[2];
};
typedef struct packState_s packStateType;

typedef struct {
  int from,to;
  char* song;
} playListItem;

void packInit(); //Generate pack list
void packSet(int packNum); //Set selected pack, default to wizznic if out of range.
void packSetByPath(const char* dir); //Search through packs to find dir, if none found, default to 0.
packStateType* packState(); //Return ptr to packstate.
const char* packGetFile(const char* path,const char* fn); //Return filename for file in currently selected pack, or from wizznic if not found there.

levelInfo_t* levelInfo(int num);
int getNumLevels();

void packFreeGfx(); //Free the images and sprites used to show packs.

void showPackBox(SDL_Surface* screen, int posx, int posy, int packNum);

int isFile(const char* fileName);
int isDir(const char* dirName);

#endif // PACK_H_INCLUDED
