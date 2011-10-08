#ifndef PARTICLES_H_INCLUDED
#define PARTICLES_H_INCLUDED

#include <SDL/SDL.h>

#include "defs.h"
#include "pixel.h"
#include "list.h"
#include "ticks.h"

#define PARTICLECOLORRANDOM 0x1e2f

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
void runParticles(SDL_Surface* screen); //This runs/draws all particle systems, and emitters.
inline void clearParticles(); //Frees all resources and removes all systems and emitters.
inline void clearSystem(pSystem_t* s); //Frees one system

#endif // PARTICLES_H_INCLUDED
