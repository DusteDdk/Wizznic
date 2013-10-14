/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2013 Jimmy Christensen <dusted@dusted.dk>             *
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

#include "defs.h"
#include "sound.h"
#include "switch.h"
#include "board.h"
#include "particles.h"
#include "draw.h"

static void switchReact( playField* pf, int x, int y ); //Should be used only private
static psysSet_t ps;

int switchSetTargets( playField* pf )
{
  //Figure out switch targets
  listItem* it = pf->levelInfo->switchList;
  while( (it=it->next) )
  {
    switch_t* sw = (switch_t*)it->data;

    //Sanity check
    if( !isSwitch( pf->board[sw->sx][sw->sy] ) )
    {
      printf("Switch error: List tells there is a switch at %i,%i but that is not the case.\n", sw->sx, sw->sy);
      free(sw);
      listRemoveItem(pf->levelInfo->switchList, it );
      continue;
    }

    //Check that the target is a supported type
    if( !switchIsValidTarget( pf, sw->dx, sw->dy ) )
    {
      printf("Switch error: Switch at %i,%i points at %i,%i but that's not a valid target brick.\n", sw->sx, sw->sy, sw->dx, sw->dy);
      free(sw);
      listRemoveItem(pf->levelInfo->switchList, it );
      continue;
    }

    switchAttachTarget( pf, sw );
    //We set the switch itself to have alive = -1 which causes it to be updated.
    pf->board[sw->sx][sw->sy]->isActive = -1;

  }

  return(1);
}

int switchFindTele( playField* pf, int x, int y )
{
  //When found, we set target =  reservedbrick then use sx/dx hack for teleport destination.
  listItem* it = pf->levelInfo->teleList;
  telePort_t* tp;

  while( (it=it->next) )
  {
    tp = (telePort_t*)it->data;
    if( tp->sx == x && tp->sy == y )
    {
      return(1);
    }
  }

  return(0);
}

int switchIsValidTarget( playField* pf, int x, int y )
{

  if( isWall( pf, x, y ) || isMover(pf->board[x][y]) || switchFindTele(pf, x, y) || ( pf->board[x][y] && (pf->board[x][y]->type==EVILBRICK || pf->board[x][y]->type==COPYBRICK || pf->board[x][y]->type==REMBRICK) ) )
  {
    return(1);
  }

 return(0);
}

void switchAttachTarget( playField* pf, switch_t* sw )
{
  //If it's a walltype or mover.
  if( isWall( pf, sw->dx, sw->dy ) || isMover(pf->board[sw->dx][sw->dy]) || (pf->board[sw->dx][sw->dy] &&(pf->board[sw->dx][sw->dy]->type==EVILBRICK||pf->board[sw->dx][sw->dy]->type==COPYBRICK||pf->board[sw->dx][sw->dy]->type==REMBRICK) ) )
  {
    pf->board[sw->sx][sw->sy]->target = pf->board[sw->dx][sw->dy];
  }

  //If it's a teleport
  if( switchFindTele( pf, sw->dx, sw->dy ) )
  {
    //So, the teleport knows that if the teleport has a target brick thats a blocker, it should look at switches, riight...
    pf->board[sw->sx][sw->sy]->target = pf->blocker;
  }

}

//Tell if a switch is disabled and pointing to x,y
int switchAmIEnabled(playField* pf, int x, int y)
{
  listItem* it=pf->levelInfo->switchList;
  switch_t* sw;
  //Do any switch have this
  while( (it=it->next) )
  {
    sw = (switch_t*)it->data;
    if( sw->dx==x && sw->dy==y )
    {
      return( pf->board[sw->sx][sw->sy]->isActive );
    }
  }

  //If no switch points to this brick, it's active.
  return(1);
}

void switchReact( playField* pf, int x, int y )
{
  int newState;

  if( y>0 && pf->board[x][y-1] && ( isBrick(pf->board[x][y-1])||isMover(pf->board[x][y-1])||pf->board[x][y-1]==pf->blockerDst ) )
  {
    newState = (pf->board[x][y]->type==SWOFF)?0:1;
  } else {
    newState = (pf->board[x][y]->type==SWOFF)?1:0;
  }

  if( pf->board[x][y]->isActive != newState )
  {
    pf->board[x][y]->isActive=newState;

    switchAffectTarget(pf, x, y, newState );
    if(newState && pf->board[x][y]->type==SWON)
    {
      sndPlay( SND_SWITCH_ACTIVATED, HSCREENW );
    } else if(pf->board[x][y]->type==SWON){
      sndPlay( SND_SWITCH_DEACTIVATED, HSCREENW );
    } else if(newState && pf->board[x][y]->type==SWOFF)
    {
      sndPlay( SND_SWITCH_DEACTIVATED, HSCREENW );
    } else if(pf->board[x][y]->type==SWOFF){
      sndPlay( SND_SWITCH_ACTIVATED, HSCREENW );
    }

  }
}

void switchAffectTarget( playField* pf, int x, int y, int newState )
{
  brickType* s = pf->board[x][y];

  switch( s->target->type )
  {
    // Walls are lifted off the board and placed in deactivated.
    case STDWALL:


      s->target->isActive = newState;

      //We turn off the brick. (if the brick is there, it might not be as we could have lifted it off, placed a brick at destination, and moved off the switch and now try to lift it again)
      if( !newState && (pf->board[ s->target->dx ][ s->target->dy ]==s->target) )
      {
        listAddData( pf->deactivated, (void*)s->target );
        pf->board[ s->target->dx ][ s->target->dy ]=NULL;
      }
      boardSetWalls( pf );
    break;

    //These types only have their active flag modified.
    case GLUE:
    case MOVERHORIZ:
    case MOVERVERT:
    case ONEWAYLEFT:
    case ONEWAYRIGHT:
    case EVILBRICK:
    case COPYBRICK:
    case REMBRICK:
      s->target->isActive = newState;
    break;

    //Teleports will watch for switches and need no modification.
    case RESERVED:

    break;

    default:
      printf( "Switch error: Type %i not handled.\n", s->target->type );
    break;
  }
  //Let's have some particles
  ps.layer=PSYS_LAYER_TOP;
  ps.x=s->target->pxx+stealGfxPtr()->tiles[s->target->type-1]->clip.w/2;
  ps.y=s->target->pxy+stealGfxPtr()->tiles[s->target->type-1]->clip.h/2;
  ps.vel=100; // +/- in each dir
  ps.life=250;
  ps.lifeVar=50;
  ps.color=PARTICLECOLORRANDOM;
  ps.numParticles=25;
  spawnParticleSystem(&ps);
}

void switchUpdateAll( playField* pf )
{
  listItem* it =pf->levelInfo->switchList;

  while( (it=it->next) )
  {
    telePort_t* sw = (telePort_t*)it->data;
    switchReact(pf, sw->sx, sw->sy );
  }
}

void switchPutBack(playField* pf)
{
  //Now we put down activated bricks from list:
  listItem* it = pf->deactivated;
  while( (it=it->next) )
  {
    brickType* b = (brickType*)it->data;
    if( b->isActive )
    {
      if(!pf->board[b->dx][b->dy])
      {
        pf->board[b->dx][b->dy]=b;
        boardSetWalls(pf);
        it=listRemoveItem(pf->deactivated,it);
      }
    }
  }
}

