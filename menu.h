#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <SDL/SDL.h>
#include "sprite.h"
#include "text.h"
#include "input.h"
#include "game.h"
#include "player.h"
#include "stars.h"
#include "leveleditor.h"
#include "particles.h"

int initMenu(SDL_Surface* screen); //Loads menu graphics
int runMenu();
void setMenu(int mstate);
void setMenuPosX(int x);
void setMenuPosY(int Y);
int getMenuState();
#endif // MENU_H_INCLUDED
