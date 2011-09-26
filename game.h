#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SDL/SDL.h>
#include "ticks.h"
#include "input.h"
#include "defs.h"
#include "sound.h"
#include "board.h"
#include "draw.h"
#include "cursor.h"
#include "sprite.h"
#include "text.h"
#include "states.h"
#include "player.h"
#include "menu.h"
#include "pack.h"

int initGame();
void cleanUpGame();
int runGame(SDL_Surface* screen);
void setGameState(int state);

#endif // GAME_H_INCLUDED
