#ifndef PARTICLES_H_INCLUDED
#define PARTICLES_H_INCLUDED

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

#include "defs.h"
#include "pixel.h"
#include "list/list.h"
#include "ticks.h"

#define PARTICLECOLORRANDOM 0x1e2f

#define PSYS_PRESET_BLACK 0
#define PSYS_PRESET_WHITE 1
#define PSYS_PRESET_COLOR 2
#define PSYS_NUM_PRESETS  3


struct particle_s
{
  uint32_t color; //Color
  int life;       //Alive? Life left
  int velx,vely;  //Velocity
  int x,y;        //Position
};
typedef struct particle_s particle_t;


struct psysSet_s
{
  int layer; //Layer in which system is active
  int x;
  int y;
  int vel; // +/- in each dir
  int life;
  int lifeVar;
  int numParticles;
  uint8_t fade;
  uint8_t gravity;
  uint8_t bounce;      //If 1, particles will bounce off screen borders ( (vel * -1)/2 )
  uint16_t fadeColor;
  uint16_t color;
  SDL_Surface* srcImg; //Getcolors for each particle from this image.
  SDL_Rect  srcRect;   //Set numpar from this, and only take colors inside this.
};
typedef struct psysSet_s psysSet_t;


struct pSystem_s
{
  particle_t *particles; //Array of particles
  psysSet_t settings;  //Settings for system
};
typedef struct pSystem_s pSystem_t;

void initParticles(SDL_Surface* screen);
void spawnParticleSystem(psysSet_t* settings); //Spawn particle system
void runParticles(SDL_Surface* screen); //Convenience function, draws all PSYS_LAYER_TOP systems
void runParticlesLayer(SDL_Surface* screen, int layer); //This runs/draws all particle systems, and emitters on LAYER
void clearParticles(); //Frees all resources and removes all systems and emitters.
void clearSystem(pSystem_t* s); //Frees one system
void psysSpawnPreset( int preset, int x, int y, int num, int life );

#endif // PARTICLES_H_INCLUDED
