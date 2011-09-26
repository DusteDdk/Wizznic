#ifndef TELEPORT_H_INCLUDED
#define TELEPORT_H_INCLUDED

#include <SDL/SDL.h>
#include "list.h"

struct telePort_s {
  int sx, sy;
  int dx, dy;
};
typedef struct telePort_s telePort_t;

void teleFreeList( listItem* l );
void teleAddToList( listItem* l, int sx, int sy, int dx, int dy );
void teleRemoveFromList( listItem* l, int sx, int sy );
void teleAddFromString(listItem* l, const char* str); //Expects a string in the format sx,sy:dx,dy
char* teleMkStrings(listItem* l);

#endif // TELEPORT_H_INCLUDED
