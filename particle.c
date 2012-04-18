/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2012 Jimmy Christensen <dusted@dusted.dk>             *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#include "particles.h"
#include "settings.h"

#define GRAVITYCONSTANT 2


static listItem* pSystems; //All particle systems are added to this list
static SDL_Surface* screen;

//Spawn particle system
void spawnParticleSystem(psysSet_t* settings)
{
  if(!setting()->particles) return;
  int i; //Gotta have a counter.

  //Create a particle system
  pSystem_t* tSystem=malloc( sizeof(pSystem_t) );

  //Copy settings
  tSystem->settings=(*settings);


  //Setup particles
  uint32_t col;
  uint8_t r,g,b;
  SDL_Surface* img;
  //Should we use info's from a surface?
  if(tSystem->settings.srcImg)
  {
    //number of particles
    tSystem->settings.numParticles=tSystem->settings.srcRect.w*tSystem->settings.srcRect.h;
    //Create array of particles
    tSystem->particles = malloc( sizeof(particle_t)*(tSystem->settings.numParticles));
    for(i=0; i<tSystem->settings.numParticles;i++)
    {

      tSystem->particles[i].life=tSystem->settings.life - (rand()%tSystem->settings.lifeVar);

      img=tSystem->settings.srcImg;

      tSystem->particles[i].x = i%tSystem->settings.srcRect.w;
      tSystem->particles[i].y = i/tSystem->settings.srcRect.w;
      col=freadPixel(img,tSystem->particles[i].x+tSystem->settings.srcRect.x,tSystem->particles[i].y+tSystem->settings.srcRect.y);

      r = ((col & img->format->Rmask) >> img->format->Rshift);
      g = ((col & img->format->Gmask) >> img->format->Gshift);
      b = ((col & img->format->Bmask) >> img->format->Bshift);
      if( !isAlpha(r,g,b) )
      {
        tSystem->particles[i].color = col;
      } else {
        tSystem->particles[i].color = col;
        tSystem->particles[i].life=0;
      }

      if(tSystem->settings.vel)
      {
        tSystem->particles[i].velx = (rand()%(tSystem->settings.vel*2))-(tSystem->settings.vel);
        tSystem->particles[i].vely = (rand()%(tSystem->settings.vel*2))-(tSystem->settings.vel);
      }

      tSystem->particles[i].x *= 100;
      tSystem->particles[i].y *= 100;
      tSystem->particles[i].x += tSystem->settings.x*100;
      tSystem->particles[i].y += tSystem->settings.y*100;
    }

  } else {
    //Create array of particles
    tSystem->particles = malloc( sizeof(particle_t)*(tSystem->settings.numParticles));

    for(i=0; i<tSystem->settings.numParticles;i++)
    {
      tSystem->particles[i].life=tSystem->settings.life - (rand()%tSystem->settings.lifeVar);
      tSystem->particles[i].velx = (rand()%(tSystem->settings.vel*2))-(tSystem->settings.vel);
      tSystem->particles[i].vely = (rand()%(tSystem->settings.vel*2))-(tSystem->settings.vel);
      tSystem->particles[i].x = tSystem->settings.x*100;
      tSystem->particles[i].y = tSystem->settings.y*100;

      if(tSystem->settings.color == PARTICLECOLORRANDOM)
      {
        tSystem->particles[i].color = SDL_MapRGB( screen->format, rand()%256,rand()%256,rand()%256);
      } else {
        tSystem->particles[i].color = tSystem->settings.color;
      }
    }
  }


  //Add to list of systems.
  listAddData(pSystems, (void*)tSystem);
}


//Just easier than having to deal with it down in the loop
inline void updateParticle(particle_t* p, SDL_Surface* screen,psysSet_t* s)
{
  //Move
  p->x += p->velx;
  p->y += p->vely;

  //Gravity
  if(s->gravity)
    p->vely += GRAVITYCONSTANT;
  //Color?
  //Fade?
  //Bounce/Edge detection
  if(p->x > (SCREENW*100) )
  {
    if(s->bounce)
    {
      p->x=(SCREENW*100);
      p->velx *= -1;
      p->velx -= p->velx/4; //Only lose 1/4 inertia
    } else {
      p->life=0;
    }
  }

  if(p->y > (SCREENH*100) )
  {
    if(s->bounce)
    {
      p->y=(SCREENH*100);
      p->vely *= -1;
      p->vely -= p->vely/3; //lose 1/3 inertia
    } else {
      p->life=0;
    }
  }
  //Age
  p->life -= getTicks();
  if(p->life<0) p->life=0;

}

//This runs/draws all particle systems, and emitters.
void runParticles(SDL_Surface* screen)
{
  runParticlesLayer(screen, PSYS_LAYER_TOP);
}

//This will not enforce PSYS_LAYER_NODRAW (it should not, no particle systems should be created if they are not to be drawn).
void runParticlesLayer(SDL_Surface* screen, int layer)
{
  if(!setting()->particles) return;
  listItem* it = pSystems;
  pSystem_t* p; //psystem
  int i;

  //Loop through systems
  while( (it=it->next) )
  {
    p=(pSystem_t*)it->data;
    if(p->settings.layer==layer)
    {

      //Draw, then update
      for( i=0; i < p->settings.numParticles; i++ )
      {
        if( p->particles[i].life )
        {
          //Draw particle
          plotPixel( screen, p->particles[i].x/100,p->particles[i].y/100, p->particles[i].color );
          //Update particle
          updateParticle(&p->particles[i], screen, &p->settings);
        }
      }
      //System life
      p->settings.life -= getTicks();
      if(p->settings.life<0)
      {
        //Remove system
        clearSystem(p);
        //Remove from list. (removeItem returns the item just before current, if any)
        it=listRemoveItem(pSystems, it);
      }
    } //System is on correct layer
  }
}

//Frees one system
inline void clearSystem(pSystem_t* s)
{
  //Free particles
  free(s->particles);
  //Free the system
  free(s);
}

//Frees all resources and removes all systems and emitters.
void clearParticles()
{
  listItem* it = pSystems;
  //Loop through systems
  while( (it=it->next) )
  {
    clearSystem( (pSystem_t*)it->data );
    it=listRemoveItem(pSystems, it);
  }
}

void initParticles(SDL_Surface* scr)
{
  pSystems = initList();
  screen=scr;
}
