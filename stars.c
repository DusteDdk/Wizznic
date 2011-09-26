#include "stars.h"
#include "pixel.h"
#include "sound.h"
#include "ticks.h"
#include "defs.h"

static listItem* stars;
static listItem* rockets;
//Setup 1000 stars
#define NUMSTARS 500
void initStars(SDL_Surface* screen)
{
  rockets = initList();
  stars = initList();
  star_t* star;
  int i;
  uint8_t col;
  for(i=0; i < NUMSTARS; i++)
  {
    star = malloc(sizeof(star_t));
    star->x = rand()%(SCREENW*10);
    star->y = rand()%(SCREENH*10);
    star->sx = rand()%10; //xpos and ypos are to be divided by 10
    star->sy = 0;
    col = rand()%230+25; //from 25 to 255
    star->color = SDL_MapRGB(screen->format, col,col,col);
    listAddData(stars, (void*)star); //Add star to list
  }
}

void starField(SDL_Surface* screen, int move)
{
  SDL_FillRect(screen, NULL, 0x00);
  star_t* star;
  listItem* it=stars;
  while( (it=it->next) )
  {
    star=(star_t*)it->data;
    //Move star
    if(move)
    {
      star->x -= star->sx;
      //Out of screen?
      if(star->x < 0)
      {
        //Give new y and reset x
        star->y=rand()%(SCREENH*10)-1;
        star->x= (SCREENW*10)-1;
      }
    }
    //Draw
    plotPixel(screen, star->x/10, star->y/10, star->color);
  }
}


/* Fireworks effect */

static int nextExpl = 0; //Countdown
void fireWorks(SDL_Surface* screen)
{
  rocket_t* tempRocket;
  star_t* tempStar;
  listItem* it;

  uint32_t colWhite = SDL_MapRGB(screen->format, 255,255,255);
  uint32_t colYellow = SDL_MapRGB(screen->format, 255,255,0);

  /*
      New Rockets
                        */
  nextExpl -= getTicks();
  if(nextExpl < 1)
  {
    //Set new timer
    nextExpl = rand()%2000;
    //Fire a new rocket
    tempRocket = malloc(sizeof(rocket_t));
    //Set initial position at y 240, and some random x
    tempRocket->y=(SCREENH*10);
    tempRocket->x=rand()%(SCREENW*10);
    //Set a direction that flies towards the middle
    tempRocket->sx = rand()%5;

    if(tempRocket->x > (HSCREENW*10) )
    {
      tempRocket->sx *= -1;
    }

    tempRocket->sy = 0-rand()%30-20;
    //Set life
    tempRocket->life=rand()%1000+250+10;

    tempRocket->p = initList();
    //Init particles for explosion
    int i, r=rand()%100;
    for(i=0; i < r; i++)
    {
      tempStar = malloc( sizeof(star_t) );
      //Set dir to something random
      tempStar->sx = rand()%500-250;
      tempStar->sy = rand()%500-250;
      tempStar->color = SDL_MapRGB( screen->format, rand()%128+128,rand()%256,rand()%128);
      tempStar->life = rand()%3000+500;
      //Add to list
      listAddData( tempRocket->p, (void*)tempStar );
    }
    //Add rocket to list
    listAddData(rockets, (void*)tempRocket);

    //Play  launch sound
    sndPlay(SND_ROCKETLAUNCH, tempRocket->x/10);
  }

  /*
      Going through rockets and their particles
                                                  */
  it=rockets;
  listItem* itt;
  while( (it=it->next) )
  {
    tempRocket=(rocket_t*)it->data;
    //If rocket is still alive, fly it
    if(tempRocket->life > 0)
    {
      //Age
      tempRocket->life -= getTicks();
      //Set position for particles if it got too old
      if(tempRocket->life < 1)
      {
        itt=tempRocket->p;
        while( (itt=itt->next) )
        {
          tempStar=(star_t*)itt->data;
          tempStar->x = tempRocket->x*10;
          tempStar->y = tempRocket->y*10;
        }
        //Play "Explosion" sound
        sndPlay(SND_ROCKETBOOM, tempRocket->x/10);
      }
      //Fly
      tempRocket->x += tempRocket->sx;
      tempRocket->y += tempRocket->sy;
      //Draw
      plotPixel(screen, tempRocket->x/10, tempRocket->y/10, colWhite );
      plotPixel(screen, tempRocket->x/10, tempRocket->y/10+1, colYellow );

    } else {
      //iterate through stars
      itt=tempRocket->p;
      int liveStars=0;
      while( (itt=itt->next) )
      {
        tempStar=(star_t*)itt->data;
        //alive?
        if(tempStar->life > 0)
        {
          //Fly

          tempStar->x += tempStar->sx/10;
          tempStar->y += tempStar->sy/10;

          //Gravity
          if(tempStar->y < 10)
              tempStar->y += 20;

          //Draw
          if(tempStar->life > 1000 || tempStar->life % 2 == 0)
            plotPixel(screen, tempStar->x/100, tempStar->y/100, tempStar->color);
          else if(tempStar->life % 3 == 0)
            plotPixel(screen, tempStar->x/100, tempStar->y/100, colWhite);

          //age
          tempStar->life -= getTicks();

          liveStars++;
        } //alive
      }
      //Check if it should still survice
      if(liveStars == 0)
      {
        //Remove stars
        itt=tempRocket->p;
        while( (itt=itt->next) )
        {
          free(itt->data);
        }
        freeList(tempRocket->p);

        //Remove rocket
        free(it->data);
        listRemoveItem(rockets, it);
      }
    } //Sim rocket stars
  } //iterate through rockets

}


