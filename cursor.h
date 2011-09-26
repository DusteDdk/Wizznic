#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#include "defs.h"

struct cursor_t
{
  int moving; //is cursor in travel
  int x,y;
  int dx,dy;
  int px,py;
  int lock; //If 1, a brick with curLock will update cursor pos
};

typedef struct cursor_t cursorType;

void updateCursor(cursorType* c); //Update cursor pixel pos if moving
int moveCursor(cursorType* c,int dirX, int dirY, int limit);
void initCursor(cursorType* c);

#endif // CONTROL_H_INCLUDED
