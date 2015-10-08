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

#include "game.h"
#include "strings.h"
#include "settings.h"
#include "defs.h"
#include "transition.h"
#include "switch.h"
#include "skipleveldialog.h"

static playField pf;
static cursorType cur;
static int countdown;
static int countdownSeconds;
static char buf[32];

static SDL_Surface* startStopImg;
#define GAMESTATEPLAYING 0
#define GAMESTATEINIT 1
#define GAMESTATEOUTOFTIME 3
#define GAMESTATECOUNTDOWN 4
#define GAMESTATEUNSOLVABLE 5
#define GAMESTATESTARTIMAGE 6
#define GAMESTATESTOPIMAGE 7
#define GAMESTATESKIPLEVEL 8
#define GAMESTATELIFELOST 9

static int gameState=GAMESTATECOUNTDOWN;
static int startStopImgCounter=0;
static int restartConfirm=0;
static psysSet_t ps;
static int timeBeforeRestart=0;
static int spentTimeBeforeRestart=0;
static SDL_Surface* ptrRestart=0;
static SDL_Rect ptrRestartRect;

static int debugNumInit=0;

static int mouseGrab=0;

static int justWon=0; //Used to detect the moment when the player won

int initGame(SDL_Surface* screen)
{
    if(player()->gameStarted)
    {
      printf("ERROR: Called init when a game was running\n");
      return(0);
    }

	//Only load the back-image once.
    if( !ptrRestart )
    {
    	ptrRestart = loadImg( DATADIR"data/ptr-restart.png" );
    	ptrRestartRect.x=HSCREENW-160;
    	ptrRestartRect.w=ptrRestartRect.x+ptrRestart->w;

    	ptrRestartRect.y=HSCREENH+120-ptrRestart->h;
    	ptrRestartRect.h=ptrRestartRect.y+ptrRestart->h;
    }

    debugNumInit++;
    initCursor(&cur);
    restartConfirm=0;

    //Read info's for level. (this is done instead of using the one in packInfo so it don't need resetting)
    pf.levelInfo = mkLevelInfo( player()->levelFile );

    if(!loadField(&pf, player()->levelFile ))
    {
      printf("Error: Couldn't init playfield.\n");
      return(0);
    }


    if(!initDraw(pf.levelInfo,screen))
    {
      printf("Error: Couldn't init graphics.\n");
      return(0);
    }

    char* buf = malloc(sizeof(char)*128);
    sprintf(buf, "themes/%s",pf.levelInfo->soundDir);
    loadSamples( buf, levelInfo( player()->level)->musicFile ); //Load samples from sounddir, note we use the levelInfo from packInfo for the music since the playlist is hacked onto that.
    free(buf);

    txtLoadGameCharSet( pf.levelInfo->fontName );

    pf.levelInfo->time *= 1000; //Convert seconds to ms
    countdown=500;
    countdownSeconds=3;
    gameState=GAMESTATECOUNTDOWN;
    player()->gameStarted=1;

    //Clear player stats
    memset( &player()->hsEntry, 0, sizeof( hsEntry_t ) );

    //Set the levelNum
    player()->hsEntry.levelNum = player()->level;

    startStopImg=0;

    if(pf.levelInfo->startImg)
    {
      gameState=GAMESTATESTARTIMAGE;
    }

    //We also simulate the first switch tick here so all looks right at the countdown.
    switchUpdateAll( &pf );

    justWon=0;

    return(1);
}

void cleanUpGame()
{
    if(!player()->gameStarted) return;

    resetBtnAll();
    cleanUpDraw();
    txtFreeGameCharSet();
    freeField(&pf);
    gameState=GAMESTATEINIT;
    player()->gameStarted=0;

    //Free memory used for levelInfo
    freeLevelInfo( &pf.levelInfo );
}

static void setGameOver()
{
  statsUpload(player()->level, player()->hsEntry.time, player()->hsEntry.moves,player()->hsEntry.combos,player()->hsEntry.score, "gameover",0, NULL);
  setMenu(menuStateGameOver);
}

void drawUi(SDL_Surface* screen)
{
   //Draw text
  char tempStr[16];

  sprintf(tempStr, "Level %i", player()->level  );
  txtWriteCenter(screen, GAMEFONTSMALL, tempStr, HSCREENW-113,HSCREENH-53);

  txtWriteCenter(screen, GAMEFONTSMALL, "Time:", HSCREENW-113,HSCREENH-36);
  int secLeft = pf.levelInfo->time/1000;
  if(pf.levelInfo->time > 60000)
  {
    sprintf(tempStr,"%i;%1.2i",secLeft/60,secLeft%60);
  } else {
    sprintf(tempStr,"%i",secLeft);
  }
  txtWriteCenter(screen, GAMEFONTMEDIUM, tempStr,HSCREENW-113, HSCREENH-26);

  txtWriteCenter(screen, GAMEFONTSMALL, "Score:", HSCREENW-114,HSCREENH+5);
  sprintf(tempStr, "%i", player()->hsEntry.score );
  txtWriteCenter(screen, GAMEFONTMEDIUM, tempStr, HSCREENW-113, HSCREENH+15);

  if( player()->lives != -1)
  {
    txtWriteCenter(screen, GAMEFONTSMALL, "Lives:", HSCREENW-113,HSCREENH+48);
    sprintf(tempStr, "%i", player()->lives );
    txtWriteCenter(screen, GAMEFONTMEDIUM, tempStr, HSCREENW-113, HSCREENH+58);
  }
}

int lostLifeMsg( cursorType* cur, playField* pf, SDL_Surface* screen, const char* strmsg, const char* straction )
{
  draw(cur,pf, screen);
  //drawUi(screen);

  countdown-=getTicks();


    txtWriteCenter(screen, GAMEFONTMEDIUM, strmsg, HSCREENW,HSCREENH-24);

    if(countdown < 1000)
    {
      txtWriteCenter(screen, GAMEFONTSMALL, STR_MENU_PRESS_B, HSCREENW,HSCREENH+12);
      //Wait for anykey
      if(getButton(C_BTNB) || countdown < -6000 || getInpPointerState()->isDown )
      {
        resetBtn(C_BTNB);
        resetMouseBtn();
        //Subtract lives
        if(!player()->inEditor)
        {
          if(player()->lives != -1)
          {
            player()->lives--;
          }

          if(player()->lives==0)
          {
            setGameOver();
          } else {
            //Lost a life, but did not get gameover, upload the death
            statsUpload(player()->level, player()->hsEntry.time, player()->hsEntry.moves,player()->hsEntry.combos,player()->hsEntry.score, straction,0, NULL);
            setMenu(menuStateNextLevel);
          }
        }

        //Clear score
          player()->hsEntry.score=0;
        //Goto cleanup, then menu
        cleanUpGame();
        startTransition(screen, TRANSITION_TYPE_ROLL_IN, 700);
        return(1);
      }
    }
    return(0);
}

void gamePause(SDL_Surface* screen)
{
  startTransition(screen, TRANSITION_TYPE_ROLL_IN,500);
  restartConfirm=0;

  //If we are in the editor, we want to jump back to last menu state
  //But otherwise we want to see the main menu.
  if(!player()->inEditor)
  {
    setMenu(menuStatePaused);
    setMenuPosY(1);
  }

}

void gameRestart(SDL_Surface* screen)
{
  restartConfirm=0;
  player()->hsEntry.score=0;
  //Save time before restarting
  timeBeforeRestart=pf.levelInfo->time;
  spentTimeBeforeRestart=player()->hsEntry.time;
  cleanUpGame();
//  startTransition(screen, TRANSITION_TYPE_DISSOLVE, 2500);
  initGame(screen);
  if(!player()->inEditor)
  {
    //Set time back to what it was before restarting
    pf.levelInfo->time=timeBeforeRestart;
    player()->hsEntry.time=spentTimeBeforeRestart;

    //Tell that we restarted
    statsUpload(player()->level, player()->hsEntry.time, player()->hsEntry.moves,player()->hsEntry.combos,player()->hsEntry.score, "reset-level",0, NULL);
  }
}

int runGame(SDL_Surface* screen)
{
  if(gameState==GAMESTATEPLAYING)
  {
    getInpPointerState()->escEnable=1;
    //Handle input
    int lim=1; //Limit cursor travel...
    int goUp=0, goDown=0, goLeft=0, goRight=0;
    if( getButton( C_UP ) )
    {
      restartConfirm=0;
      if( getBtnTime( C_UP ) > REPEATDELAY )
      {
        goUp=1;
      } else if(getBtnTime(C_UP)==0) {
        goUp=1;
        lim=0;
      }
    }

    if( getButton( C_DOWN ) )
    {
      restartConfirm=0;
      if( getBtnTime( C_DOWN ) > REPEATDELAY )
      {
        goDown=1;
      } else if(getBtnTime(C_DOWN)==0) {
        goDown=1;
        lim=0;
      }
    }

    if( getButton( C_LEFT ) )
    {
      restartConfirm=0;
      if( getBtnTime( C_LEFT ) > REPEATDELAY )
      {
        goLeft=1;
      } else if(getBtnTime(C_LEFT)==0) {
        goLeft=1;
        lim=0;
      }
    }

    if( getButton( C_RIGHT ) )
    {
      restartConfirm=0;
      if( getBtnTime( C_RIGHT ) > REPEATDELAY )
      {
        goRight=1;
      } else if(getBtnTime(C_RIGHT)==0) {
        goRight=1;
        lim=0;
      }
    }

    //Pause ?
    if( getButton( C_BTNMENU ) || isPointerEscapeClicked() )
    {
      resetBtn( C_BTNMENU );
      gamePause(screen);
      return(STATEMENU);
    }

    //Retry
    if( getButton( C_BTNSELECT ) || (getInpPointerState()->timeSinceMoved<POINTER_SHOW_TIMEOUT && isBoxClicked(&ptrRestartRect)) )
    {
      resetBtn( C_BTNSELECT );
      resetMouseBtn();
      if(!restartConfirm)
      {
        restartConfirm=1;
      } else if(restartConfirm) {
        gameRestart(screen);
      }

    }

    //Handle mouse input
    if( getInpPointerState()->timeSinceMoved==0 && !cur.lock )
    {
      setCursor(&cur, getInpPointerState()->curX,getInpPointerState()->curY );
    }

    if(!getInpPointerState()->isDown)
    {
      mouseGrab=0;
      //Allow moving the cursor around with the input device when no brick is below, just for effect
    } else {
      brickType* b=brickUnderCursor(&pf, cur.x,cur.y);

      //We're over a brick, tell curser it's position, it will be locked later because we grab it now

      if( b )
      {
        getInpPointerState()->startX=b->dx;
        getInpPointerState()->startY=b->dy;
        if( !cur.lock )
        {
          mouseGrab=1;
          getInpPointerState()->startX=getInpPointerState()->curX;
          getInpPointerState()->startY=getInpPointerState()->curY;
        } else
        {
          if( b->dx > getInpPointerState()->curX )
          {
            //Drag Left
            if( b->dx == b->sx  && getInpPointerState()->startX != getInpPointerState()->curX )
            {
              goLeft=1;
            }
          } else if(  b->dx < getInpPointerState()->curX )
          {
            //Drag Right
            if( b->dx == b->sx && getInpPointerState()->startX != getInpPointerState()->curX )
            {
              goRight=1;
            }
          }
        }
      } else {
        mouseGrab=0;
      }
    }
   //   printf("x:%i\n", getInpPointerState()->curX );
      //Drag
    if( getButton( C_BTNX ) || getButton( C_BTNB ) || mouseGrab || isPointerClicked() )
    {
      //Remove "Restart" question
      restartConfirm=0;

      //Magnet to brick if it's moving
      brickType* b=brickUnderCursor(&pf, cur.dx, cur.dy);

      if( !cur.lock && b )
      {
        //Attach cursor
        cur.lock=1;
        b->curLock=1;
        cur.x=cur.dx;
        cur.y=cur.dy;
        cur.px = b->pxx-4;
        cur.py = b->pxy-4;

        sndPlay( SND_BRICKGRAB, cur.px );
      }

      int movedBrick=0;
      //We're holding a brick, and it's not falling
      if( b )
      {
        if( (goRight && curMoveBrick(&pf,b, DIRRIGHT)) || (goLeft && curMoveBrick(&pf,b, DIRLEFT)) )
        {
          movedBrick=1;
          b->curLock=1;
          cur.lock=1;
        }
      }

      //Moved brick
      if(movedBrick)
      {
        player()->hsEntry.moves++;
        sndPlay(SND_BRICKMOVE, cur.px);
        ps.layer=PSYS_LAYER_TOP;
        ps.x=b->pxx;
        ps.y=b->pxy+18;
        ps.vel=50;
        ps.life=500;
        ps.lifeVar=250;
        ps.gravity=1;
        ps.srcImg=stealGfxPtr()->tiles[b->type-1]->img;
        ps.srcRect=stealGfxPtr()->tiles[b->type-1]->clip;
        ps.srcRect.y += 18;
        ps.srcRect.h = 2;
        spawnParticleSystem(&ps);
      }

    }
    else
    {
      cur.lock=0;
    }

      if(!cur.lock)
      {
        if( goLeft ) moveCursor(&cur, DIRLEFT, 0, lim);
        if( goRight ) moveCursor(&cur, DIRRIGHT, 0, lim);
        if( goUp ) moveCursor(&cur, 0, DIRUP, lim);
        if( goDown ) moveCursor(&cur, 0, DIRDOWN, lim);
      }

    //Sim first, so moving blocks get evaluated before getting moved again
    simField(&pf, &cur);

    //Do rules
    int ret=doRules(&pf);

    //Draw scene
    draw(&cur,&pf, screen);

    //Draw a path to show where we are pulling the brick
    if( mouseGrab )
      drawPath( screen, getInpPointerState()->startX,getInpPointerState()->startY,getInpPointerState()->curX,getInpPointerState()->startY,1 );


    //If no more bricks, countdown time left.
    if(ret == NOBRICKSLEFT)
    {
      if( !justWon )
      {
        sndPlay(SND_WINNER,160);
      }
      justWon++;
      pf.levelInfo->time -= 1000;
      player()->hsEntry.score +=1;

      if(getButton(C_BTNX) || getButton(C_BTNB) || isPointerClicked() )
      {
        resetBtn(C_BTNX);
        resetBtn(C_BTNB);
        resetMouseBtn();
        while(pf.levelInfo->time > 0)
        {
          player()->hsEntry.score +=1;
          pf.levelInfo->time -= 1000;
        }
      }

      if(justWon > 50)
      {
        sndPlayOnce(SND_SCORECOUNT, 160);
      }
      if(pf.levelInfo->time < 1)
      {
        //Completed level
        player()->timeouts=0;
        pf.levelInfo->time=0;
        sndPlay(SND_VICTORY, 160);

        if(!player()->inEditor)
        {
          //Don't submit if it was from the leveleditor
          statsSubmitBest();
          setMenu(menuStateFinishedLevel);
          if(pf.levelInfo->stopImg)
          {
            gameState=GAMESTATESTOPIMAGE;
            return(STATEPLAY);
          }
        } else {
          setLevelCompletable(pf.levelInfo->file, 1);
        }
        cleanUpGame();
        startTransition(screen, TRANSITION_TYPE_ROLL_IN, 700);
        return(STATEMENU);
      }
    } else if(ret > 0) //Player destroyed bricks.
    {
      if(ret > 2) //Check for combo's
      {
        ///TODO: Some nice text effect? How about dissolving an image into a particle system?
        printf("%i Combo!\n",ret);
        player()->hsEntry.combos++;
      }
      player()->hsEntry.score += ret*ret*11*(player()->level+1);
    }

    //if ret > -1 then ret == number of bricks destroyed
    if(ret>-1)
    {
      //Update time:
      pf.levelInfo->time -= getTicks();
      player()->hsEntry.time += getTicks();
      if(pf.levelInfo->time < 1 && ret!=NOBRICKSLEFT )
      {
        countdown=4000;
        gameState=GAMESTATEOUTOFTIME;
        if( !player()->inEditor )
        {
          player()->timeouts++;
        }

        sndPlay(SND_TIMEOUT, 160);
      }
    }

    //Check if level is unsolvable.
    if(ret==UNSOLVABLE)
    {
      countdown=2000;
      gameState=GAMESTATEUNSOLVABLE;
      if( !player()->inEditor )
      {
        player()->timeouts++;
      }

      sndPlay(SND_LOSER, 160);
    } else if(ret==LIFELOST)
    {
      countdown=2000;
      gameState=GAMESTATELIFELOST;

      if( !player()->inEditor )
      {
        player()->timeouts++;
      }

      sndPlay(SND_LOSER, 160);

    }


    //Draw question
    if(restartConfirm)
    {
      sprintf(buf,STR_GAME_RESTARTWARNING);
      txtWriteCenter(screen, GAMEFONTMEDIUM, buf, HSCREENW, HSCREENH-20);
      sprintf(buf,STR_GAME_RESTARTCONFIRM);
      txtWriteCenter(screen, GAMEFONTSMALL, buf, HSCREENW, HSCREENH);
    } else {
      //Draw text
      drawUi(screen);
    }
    //Show the restart icon
    if(getInpPointerState()->timeSinceMoved<POINTER_SHOW_TIMEOUT && getInpPointerState()->escEnable)
    {
      SDL_Rect ptrRestartRectC = ptrRestartRect;
      SDL_BlitSurface( ptrRestart,NULL, screen, &ptrRestartRectC );
    }


  } else
  if(gameState==GAMESTATECOUNTDOWN)
  {

    draw(&cur,&pf, screen);
    countdown -=getTicks();

    if( getButton( C_BTNMENU ) )
    {
      resetBtn( C_BTNMENU );
      countdownSeconds=0;
      countdown=0;
    }

    if( (getButton( C_BTNX ) || getButton( C_BTNB ) || getInpPointerState()->isDown ) && countdownSeconds )
    {
      countdownSeconds=0;
      countdown=500;
    }

    drawShowCountDown(screen, countdownSeconds);

    drawUi(screen);

    if(countdown < 1)
    {
      countdown=1000;
      countdownSeconds--;

      if(countdownSeconds == -1)
      {
        gameState=GAMESTATEPLAYING;
        return(STATEPLAY);
      }
      if(countdownSeconds==0)
      {
        countdown=500;
        sndPlay(SND_START, 160);
      } else {
        sndPlay(SND_COUNTDOWNTOSTART, 160);
      }
    }

  } else
  if(gameState==GAMESTATEOUTOFTIME) //Menu was last in "Entering level" so it will return to that if timeout
  {
    draw(&cur,&pf, screen);
    //drawUi(screen);

    countdown-=getTicks();

    //Offer to skip after dying twice on same level, but only if it is not the last level in the pack.
    if( player()->timeouts > 1 && player()->level+1 < getNumLevels() )
    {
      int skipLevel = skipLevelDialog(screen);
      if( skipLevel==1 )
      {
        gameState=GAMESTATESKIPLEVEL;
        countdown=500;
      }
      txtWriteCenter(screen, GAMEFONTMEDIUM, STR_GAME_OUTOFTIME, HSCREENW,HSCREENH-24-31);
    } else {
      if( lostLifeMsg(&cur, &pf, screen, STR_GAME_OUTOFTIME, "lostlife-timeout" ) )
      {
        return(STATEMENU);
      }
    }

  } else
  if(gameState==GAMESTATEUNSOLVABLE) //The same as out-of-time, but with another graphics.
  {
    draw(&cur,&pf, screen);
    //drawUi(screen);

    countdown-=getTicks();

    //Offer to skip after dying twice on same level, but only if it is not the last level in the pack.
    if( player()->timeouts > 1 && player()->level+1 < getNumLevels() )
    {
      int skipLevel = skipLevelDialog(screen);
      if( skipLevel==1 )
      {
        gameState=GAMESTATESKIPLEVEL;
        countdown=500;
      }
      txtWriteCenter(screen, GAMEFONTMEDIUM, buf, HSCREENW,HSCREENH-24-31);

    } else {
      if( lostLifeMsg(&cur, &pf, screen, STR_GAME_UNSOLVABLE, "lostlife-unsolvable" ) )
      {
        return(STATEMENU);
      }
    }


  } else
  if(gameState==GAMESTATELIFELOST)
  {
    if( lostLifeMsg(&cur, &pf, screen, STR_GAME_LOSTLIFE, "lostlife-evilbrick" ) )
    {
      return(STATEMENU);
    }
  } else
  if(gameState==GAMESTATESTARTIMAGE)
  {

    if(!startStopImg)
    {
      startStopImgCounter=0;
      startStopImg = loadImg( packGetFile("themes/", pf.levelInfo->startImg) );
      if(!startStopImg)
      {
        printf("Couldn't load '%s'\n",packGetFile("themes/", pf.levelInfo->startImg));
      }
    }

    startStopImgCounter+=getTicks();

    if((startStopImgCounter > 500 && ( getButton(C_BTNB) || isPointerClicked() ) ) || !startStopImg)
    {
      if(startStopImg)
        SDL_FreeSurface(startStopImg);
      startStopImg=0;
      resetBtn(C_BTNB);
      resetMouseBtn();
      gameState=GAMESTATECOUNTDOWN;
    }

    SDL_BlitSurface( startStopImg, 0, screen, &(setting()->bgPos) );

    if(startStopImgCounter>4000)
      txtWriteCenter(screen, GAMEFONTSMALL, STR_GAME_PRESSB, HSCREENW,HSCREENH+80);

  } else if(gameState==GAMESTATESTOPIMAGE)
  {
    if(!startStopImg)
    {
      startStopImgCounter=0;
      startStopImg = loadImg( packGetFile("themes/", pf.levelInfo->stopImg) );
      if(!startStopImg)
      {
        printf("Couldn't load '%s'\n",packGetFile("themes/", pf.levelInfo->stopImg));
      }
    }

    startStopImgCounter+=getTicks();

    if((startStopImgCounter > 500 && (getButton(C_BTNB) || isPointerClicked() ) ) || !startStopImg)
    {
      if(startStopImg)
        SDL_FreeSurface(startStopImg);
      startStopImg=0;
      resetBtn(C_BTNB);
      resetMouseBtn();
      cleanUpGame();
      startTransition(screen, TRANSITION_TYPE_ROLL_IN, 700);
      return(STATEMENU);
    }

    SDL_BlitSurface( startStopImg, 0, screen, &(setting()->bgPos) );
    if(countdownSeconds>4000)
      txtWriteCenter(screen, GAMEFONTSMALL, STR_GAME_PRESSB, HSCREENW,HSCREENH+80);

  } else if(gameState==GAMESTATESKIPLEVEL)
  {

    doRules(&pf);
    simField(&pf, &cur);
    draw(&cur,&pf, screen);

    countdown-=getTicks();

    if( countdown < 1 )
    {
      countdown=500;

      if( boardDestroyNextBrick(&pf) == 0 )
      {

        //Tell that we chose to skip level
        statsUpload(player()->level, player()->hsEntry.time, player()->hsEntry.moves,player()->hsEntry.combos,player()->hsEntry.score, "skip-level",0, NULL);

        pf.levelInfo->time=0;
        player()->hsEntry.score=0;
        cleanUpGame();
        startTransition(screen, TRANSITION_TYPE_ROLL_IN, 700);
        clearParticles();
        setMenu(menuStatePrepareNextLevel);
        return(STATEMENU);
      }
    }
    drawUi(screen);

  }
  return(STATEPLAY);
}
