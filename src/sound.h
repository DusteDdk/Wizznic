#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

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

#include <SDL/SDL.h>

#define SND_MENUMOVE 0
#define SND_MENUCLICK 1
#define SND_FIRST_GAME_SAMPLE 2
#define SND_CLICK 2
#define SND_BRICKMOVE 3
#define SND_BRICKLAND 4
#define SND_BRICKBREAK 5
#define SND_BRICKGRAB 6
#define SND_SCORECOUNT 7
#define SND_ROCKETBOOM 8
#define SND_ROCKETLAUNCH 9
#define SND_COUNTDOWNTOSTART 10
#define SND_START 11
#define SND_TIMEOUT 12
#define SND_VICTORY 13
#define SND_ONEWAY_MOVE 14
#define SND_TELEPORTED 15
#define SND_LAST_GAME_SAMPLE 15
#define SND_SWITCH_ACTIVATED 16
#define SND_SWITCH_DEACTIVATED 17
#define SND_BRICKSWAP 18
#define SND_BRICKSWAP_DENIED 19
#define SND_BRICKCOPY 20
#define SND_BRICKCOPY_DENIED 21
#define SND_WINNER 22
#define SND_LOSER 23
#define NUMSAMPLES 24


int initSound();
void loadSamples(const char* sndDir, const char* musicFile); //Loads all samples for the game
void loadMenuSamples(); //Loads all samples for the game
void sndPlay(int sample, int posX);
void sndPlayOnce(int sample, int posX);
void sndChangeState(int toState); //Changes from in-game to menu music
void soundSetMusic(); //Sets music to either in-game or to user-selected.
void soundRun(SDL_Surface* screen, int state);
void soundPlayUserSongNum(int num, char* songName);
void soundSetMusVol(int v);

#endif // SOUND_H_INCLUDED
