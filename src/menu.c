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

#include <math.h>

#include "menu.h"

#include "userfiles.h"

#include "sound.h"
#include "ticks.h"
#include "input.h"
#include "strinput.h"

#include "states.h"
#include "game.h"
#include "levelselector.h"

#include "pack.h"
#include "strings.h"
#include "settings.h"

#include "stats.h"
#include "mbrowse.h"

#include "credits.h"

#include "waveimg.h"
#include "stats.h"
#include "scrollbar.h"
#include "about.h"

#include "transition.h"

#if defined( PLATFORM_SUPPORTS_STATSUPLOAD )
  #include "bundle.h"
  #include "platform/libDLC.h"
#endif

static float rot=0;

#define MENUGFXINTRO 0
#define MENUGFXHELP 1
#define MENUGFXBYE 2
#define MENUGFXPACKBOX 3
#define MENUGFXNUM 4

static SDL_Surface* menuBg[MENUGFXNUM];


static int menuState=menuStateIntro;

static int menuPosX=0, menuPosY=0;
static int menuMaxX, menuMaxY;
static int menuChangeX=0,menuChangeY=0; //Is not zero if there's a change

static int dir=0;
static int countdown=0;

static int menuReturnHack=0; //Well, for returning to the right place after entering highscore.

static int x,y;

static wavingImage_t waving;

static aniType* menuYesNo;

inpStrState* strStateHighScore;
inpStrState* strStateDlcCode;

char message[128];

int getMenuState() { return(menuState); }

int initMenu(SDL_Surface* screen)
{
  menuBg[MENUGFXINTRO] = loadImg( DATADIR"data/menu/intro.png" );
  menuBg[MENUGFXHELP] = loadImg( DATADIR""PLATFORM_HELP_FILE );

  scrollBarInit();
  aboutInit();

  menuBg[MENUGFXBYE]=0;
  menuYesNo=0;

  menuBg[MENUGFXPACKBOX] = SDL_CreateRGBSurface(SDL_SWSURFACE, 260,42, (setting()->bpp*8), screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,0xff000000);

  setWaving(&waving, screen, menuBg[MENUGFXINTRO], HSCREENW-149,HSCREENH-90,1,15,300);
  waving.privRotAmount=0; //In case it was nan
  waving.useOverlay=1;
  waving.overlaySpeed=1;
  waving.overlay = loadImg( DATADIR"data/menu/introoverlay.png" );
  waving.overlayPos=waving.overlay->w+1;
  waving.mask = loadImg( DATADIR"data/menu/intromask.png");

  strStateHighScore = inpStrCreate(screen, "Name:", 1,10 );
  strStateDlcCode = inpStrCreate(screen, "Code:", 1,20 );

  return(1);
}


void setMenu(int mstate)
{
  menuPosY=0;
  menuPosX=0;
  menuState=mstate;
  dir=0;
  countdown=0;
}

void setMenuPosX(int x)
{
  menuPosX=x;
}
void setMenuPosY(int Y)
{
  menuPosX=Y;
}

void decPosX()
{
  menuChangeX=1;
  sndPlay(SND_MENUMOVE, 160);

  menuPosX--;
  if(menuPosX < 0)
    menuPosX=menuMaxX;
}

void incPosX()
{
  menuChangeX=1;
  sndPlay(SND_MENUMOVE, 160);
  menuPosX++;
  if(menuPosX > menuMaxX)
    menuPosX=0;
}

void decPosY()
{
  menuChangeY=-1;
  if(!( menuState==menuStatePaused && menuPosY==1 && !player()->gameStarted ))
    sndPlay(SND_MENUMOVE, 160);
  countdown=0;
  dir=0;
  menuPosY--;
  if(menuPosY < 0)
    menuPosY=menuMaxY;
}

void incPosY()
{
  menuChangeY=1;
  if(!( menuState==menuStatePaused && menuPosY==1 && !player()->gameStarted ))
    sndPlay(SND_MENUMOVE, 160);
  countdown=0;
  dir=0;
  menuPosY++;
  if(menuPosY > menuMaxY)
    menuPosY=0;
}

int runMenu(SDL_Surface* screen)
{
  char buf[512];
  int scroll; //Generic scoll int (for scrolling lists)
  int ul=0;   //Userlevel (and used for scrolling)
  psysSet_t ps; //Particle system for particle effects in menu
  listItem* it;
  fileListItem_t* fItem;
  int sb;

  SDL_Rect r;

  if( !dir || getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT ) //Nasty little hack to prevent text from blinking when you try to click it
  {
    countdown+=getTicks();
    if(countdown > 250)
    {
      countdown=250;
      dir=1;
    }
  } else {
    countdown-=getTicks();
    if(countdown < 1)
    {
      countdown=0;
      dir=0;
    }
  }

  if(getButton(C_BTNB))
    sndPlayOnce(SND_MENUCLICK, 160);

  if(getButton(C_UP) )
  {
    resetBtn(C_UP);
    getInpPointerState()->timeSinceMoved=POINTER_SHOW_TIMEOUT;
    decPosY();
    if( menuState==menuStatePaused && menuPosY==1 && !player()->gameStarted )
    {
      decPosY();
    }
  }
  if(getButton(C_DOWN))
  {
    resetBtn(C_DOWN);
    getInpPointerState()->timeSinceMoved=POINTER_SHOW_TIMEOUT;
    incPosY();
    if( menuState==menuStatePaused && menuPosY==1 && !player()->gameStarted )
    {
      incPosY();
    }
  }

  if(getButton(C_LEFT) )
  {
    resetBtn(C_LEFT);
    getInpPointerState()->timeSinceMoved=POINTER_SHOW_TIMEOUT;
    decPosX();
  }
  if(getButton(C_RIGHT))
  {
    resetBtn(C_RIGHT);
    getInpPointerState()->timeSinceMoved=POINTER_SHOW_TIMEOUT;
    incPosX();
  }

  switch(menuState)
  {
    case menuStateIntro:

//Intro should no longer crash PSP, will porter please get back to me on this?
//Intr not working on PSP.. Skip it untill its fixed.
//      #if defined (PSP) && !defined (PSP_DEBUG_INTRO)
//        menuState=menuStatePaused;
//        SDL_FreeSurface( menuBg[MENUGFXINTRO] );
//        menuBg[MENUGFXINTRO]=0;
//        menuPosY=0;
//        clearParticles();
//        clearCredits();
//        break;
//      #endif

      //Show background image
      starField(screen,1);

      runParticles(screen);

      runCredits(screen);

      waveImg(&waving);

      //Blink "Press B" (NOT dir, it will disappear when mouse is moved)
      if(!dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+70);

      //Show version
      txtWrite(screen, FONTSMALL, VERSION_STRING, HSCREENW+160-((strlen(VERSION_STRING))*9),HSCREENH+106);

      //Show games played world wide
      if( setting()->solvedWorldWide )
      {
        sprintf(buf, STR_MENU_PUZZLES_WORLDWIDE,setting()->solvedWorldWide);
        txtWrite(screen, FONTSMALL, buf, HSCREENW-158, HSCREENH+95);
      }

      //Wait for keypress
      if( getButton( C_BTNB ) || isPointerClicked() )
      {
        resetBtn( C_BTNB );
        menuState=menuStatePaused;
        SDL_FreeSurface( menuBg[MENUGFXINTRO] );
        SDL_FreeSurface( waving.overlay  );
        SDL_FreeSurface( waving.mask ) ;

        startTransition( screen, TRANSITION_TYPE_DISSOLVE, 450 );

        menuBg[MENUGFXINTRO]=0;
        menuPosY=0;
        //clearParticles();
        clearCredits();
        #if defined (PLATFORM_SUPPORTS_STATSUPLOAD)
        if(setting()->firstRun)
        {
          setMenu(menuStateUploadDiag);
          menuPosX=1; //Default to "choose"
        }
        if(setting()->session == -1)
        {
          menuState=menuStateUpdate;
        }
        #endif
      } else
      if( getButton( C_BTNY ) )
      {
        resetBtn( C_BTNY );
        ps.layer=PSYS_LAYER_TOP;
        ps.x=HSCREENW-149;
        ps.y=HSCREENH;
        ps.vel=100; // +/- in each dir
        ps.life=2000;
        ps.lifeVar=1000;
        ps.fade=0;
        ps.gravity=0;
        ps.bounce=1;      //If 1, particles will bounce off screen borders ( (vel * -1)/2 )
        ps.fadeColor=0x00;
        ps.color=PARTICLECOLORRANDOM;
        ps.srcImg=menuBg[MENUGFXINTRO]; //Getcolors for each particle from this image.
        ps.srcRect.x=0;
        ps.srcRect.y=0;
        ps.srcRect.w=menuBg[MENUGFXINTRO]->w;
        ps.srcRect.h=menuBg[MENUGFXINTRO]->h;
        spawnParticleSystem(&ps);
      }

    break; //Intro screen

    case menuStateNewGame: //Start new game (select starting level)
      starField(screen,1);
      getInpPointerState()->escEnable=1;

      menuMaxX = stats()->progress+2;

      //To make sure that we don't try to show a level that's not there
      if(menuMaxX > getNumLevels())
      {
        menuMaxX=getNumLevels();
      }
      if(menuPosX > getNumLevels())
      {
        menuPosX = getNumLevels();
      }

      if(menuPosX-1 < stats()->progress &&  menuPosX!=getNumLevels())
      {
        txtWrite(screen, FONTMEDIUM, ">>", HSCREENW+120,HSCREENH-12);
        if( isBoxClicked( getTxtBox() ) )
        {
          incPosX();
        }
      }

      if(menuPosX > 0)
      {
        txtWrite(screen, FONTMEDIUM, "<<", HSCREENW-155, HSCREENH-12);
        if( isBoxClicked( getTxtBox() ) )
        {
          decPosX();
        }
      }

      //The "Finished" image..
      if(menuPosX == menuMaxX)
      {
        levelSelector(screen,menuPosX, 0);
      } else {
        levelSelector(screen,menuPosX, 1);
        txtWriteCenter(screen, FONTMEDIUM, STR_MENU_NEW_GAME, HSCREENW, HSCREENH-100);
        txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B_PLAY, HSCREENW, HSCREENH+108);

        if( ( (isPointerClicked() && !isAnyBoxHit()) || getButton(C_BTNB) ) && menuPosX!=getNumLevels())
        {
          resetBtn(C_BTNB);

          initPlayer(); //New game, reset player.
          player()->levelFile = levelInfo( menuPosX )->file;
          player()->level = menuPosX;

          // Show help if it's the first level
          menuState=menuStateNextLevel;
        }
      }

      if(getButton(C_BTNMENU) || isPointerEscapeClicked() )
      {
        resetBtn(C_BTNMENU);
        menuPosY=0;
        startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );

        setMenu(menuStatePaused);
        break;
      }

    break; // New game

    case menuStateNextLevel:
      starField(screen,0);
      getInpPointerState()->escEnable=1;

      levelSelector(screen, player()->level,1);


      if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B_PLAY, HSCREENW, HSCREENH+108);

      if( getButton(C_BTNMENU) || isPointerEscapeClicked() )
      {
        resetBtn(C_BTNMENU);
        menuState=menuStateNewGame;
        if( statsIsHighScore() )
        {
          setMenu(menuStateEnterHighScore);
          menuReturnHack=menuStateNewGame;
        }
      } else
      if(getButton(C_BTNB) || isPointerClicked() )
      {
        resetBtn(C_BTNB);
        resetMouseBtn();

        if(initGame(screen))
        {
          if(player()->level == 0)
          {
            setMenu(menuStateHowto);
          } else {
            startTransition(screen, TRANSITION_TYPE_ROLL_OUT,500);
            return(STATEPLAY);
          }
        } else {
          printf("Fatal: Couldn't init game. \n");
          return(STATEQUIT);
        }
      }

    break;

    case menuStateFinishedLevel:
      starField(screen,0);
      fireWorks(screen);
      sprintf(buf, "Level %i Done!", player()->level);
      txtWave(screen, FONTMEDIUM,buf, HSCREENW, HSCREENH-95, &rot);

      sprintf(buf, "Progress: %.1f%%",  ((100.0f/(float)getNumLevels())*((float)player()->level+1.0f)) );
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-60);

      sprintf(buf, "%i pts total",  player()->campStats.score);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-40);

      sprintf(buf, "Stats for level %i", player()->level);
      txtWriteCenter(screen, FONTSMALL, buf,HSCREENW,HSCREENH);
      txtWrite(screen, FONTSMALL, " Stat      Yours         Best     ", (HSCREENW-153),(HSCREENH+20) );
      txtWrite(screen, FONTSMALL, "+-----+-------------+------------+", (HSCREENW-153),(HSCREENH+30) );
      txtWrite(screen, FONTSMALL, "Score :", (HSCREENW-153),(HSCREENH+42) );
      txtWrite(screen, FONTSMALL, "Time  :", (HSCREENW-153),(HSCREENH+54) );
      txtWrite(screen, FONTSMALL, "Moves :", (HSCREENW-153),(HSCREENH+68) );
      txtWrite(screen, FONTSMALL, "Combos:", (HSCREENW-153),(HSCREENH+80) );

      //levelscore
      sprintf(buf, "%i", player()->hsEntry.score);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+42);

      //best levelscore
      sprintf(buf, "%i", stats()->cl->score);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+42);

      //time
      sprintf(buf, "%i:%i", player()->hsEntry.time/60,player()->hsEntry.time%60 );
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+54);

      //best time
      sprintf(buf, "%i:%i", stats()->cl->time/60,stats()->cl->time%60 );
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+54);

      //Moves
      sprintf(buf, "%i", player()->hsEntry.moves);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+68);

      //best moves
      sprintf(buf, "%i", stats()->cl->moves);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+68);

      //Combos
      sprintf(buf, "%i", player()->hsEntry.combos);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+80);
      //best moves
      sprintf(buf, "%i", stats()->cl->combos);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+80);

      txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+108);

      if(getButton(C_BTNB) || isPointerClicked() )
      {
        resetBtn(C_BTNB);

        setMenu( menuStatePrepareNextLevel );
        return(STATEMENU);
      }

    break;

    case menuStatePrepareNextLevel:
      //Reset board score
      player()->hsEntry.score=0;

      //Check if the player compleated all levels
      if(player()->level+1 == getNumLevels())
      {
        //Go to "Completed pack" image.
        setMenu(menuStateBeatPack);
        break;
      }

      //Inc to next lvl
      player()->level++;

      //Set new level
      player()->levelFile = levelInfo( player()->level )->file;

      //Entering screen
      menuState=menuStateNextLevel;
    break;

    case menuStatePaused:
      menuMaxY = 8;
      starField(screen,1);
      if(player()->gameStarted)
      {
        txtWave(screen, FONTMEDIUM, STR_MENU_PAUSE_HEADLINE, HSCREENW, HSCREENH-115, &rot);
      } else {
        txtWave(screen, FONTMEDIUM, STR_MENU_WIZZNIC_HEADLINE, HSCREENW, HSCREENH-115, &rot);
      }

      if(dir || menuPosY!= 0)
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_NEW_GAME_CHOICE, HSCREENW, HSCREENH-80);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=0;
      }

      if( player()->gameStarted && (dir || menuPosY!= 1 ) )
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_RESUME_CHOICE, HSCREENW, HSCREENH-60);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=1;
      }

      if(dir || menuPosY!= 2)
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_HIGHSCORES_CHOICE, HSCREENW, HSCREENH-40);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=2;
      }

      if(dir || menuPosY!= 3)
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_OPTIONS_CHOICE, HSCREENW, HSCREENH-20);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=3;
      }

      if(dir || menuPosY!= 4)
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_EDITOR_CHOICE, HSCREENW, HSCREENH+0);
       if( isBoxClicked( getTxtBox() ) )
       {
          menuPosY=4;
       }
      }

      if(dir || menuPosY!= 5)
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_PACK_CHOICE, HSCREENW, HSCREENH+20);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=5;
      }

      if(dir || menuPosY!= 6)
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_ABOUT_CHOICE, HSCREENW, HSCREENH+40);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=6;
      }


      if(dir || menuPosY!= 7)
      {
        txtWriteCenter(screen, FONTSMALL, STR_MENU_EXIT_CHOICE, HSCREENW, HSCREENH+60);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=7;
      }

      if(dir || menuPosY!= 8)
      {
        sprintf(buf, STR_MENU_PACK_SHOW, packState()->cp->name);
        txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+100);
        if( isBoxClicked( getTxtBox() ) )
          menuPosY=8;
      }


      if( getButton( C_BTNB ) || isAnyBoxHit() )
      {
        resetBtn( C_BTNB );
        resetMouseBtn();

        switch(menuPosY)
        {
          case 0: //Newgame
            //If in arcade-mode, clear progress before entering level-selection.
            if(setting()->arcadeMode)
            {
              stats()->progress=-1;
            }

            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );
            setMenu(menuStateNewGame);

            //Clean up.
            if(player()->gameStarted)
            {
              cleanUpGame();
              //Check if highscore, if a game was started
              if( statsIsHighScore() )
              {
                setMenu(menuStateEnterHighScore);
                menuReturnHack=menuStateNewGame;
              }
            }

            //Current selected level.
            menuPosX=stats()->progress+1;
          break;
          case 1: //Resume, unset paused, return to game
            if(player()->gameStarted)
            {
              startTransition(screen, TRANSITION_TYPE_ROLL_OUT,500);
              return(STATEPLAY);
            }
          break;
          case 2: //Highscores
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );
            setMenu(menuStateHighScores);
          break;
          case 3: //Options
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );
            setMenu(menuStateOptions);
          break;
          case 4: //UserLevels
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );
            setMenu(menuStateUserLevels);
            if(player()->gameStarted) cleanUpGame();
            return(STATEMENU);
          break;
          case 6: //Switch to about screen
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );
            setMenu(menuStateAbout);
          break;

          case 7: //Exit program
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );
            if( statsIsHighScore() )
            {
              setMenu(menuStateEnterHighScore);
              menuReturnHack=menuStateOutro;
            } else {
              setMenu(menuStateOutro);
            }
          break;
          case 5:
          case 8: //Pack selection
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );
            setMenu(menuStatePackList);
            menuPosY = packState()->selected;
          break;
        }
      }
      break;

      case menuStateHowto:
        starField(screen,1);

        #if defined (GP2X) || defined (PSP) || defined (WIZ)
        //Sprinkle particles
        if( !dir && countdown < 1 )
        {
          ps.layer=PSYS_LAYER_TOP;
          ps.x=(rand()%109)+107;
          ps.y=(rand()%40)+78;
          ps.vel=50; // +/- in each dir
          ps.life=2000;
          ps.lifeVar=500;
          ps.fade=0;
          ps.gravity=1;
          ps.bounce=0;      //If 1, particles will bounce off screen borders ( (vel * -1)/2 )
          ps.fadeColor=0x00;
          ps.color=PARTICLECOLORRANDOM;
          ps.srcImg=0; //Don't get colors from an image
          ps.numParticles=20;
          spawnParticleSystem(&ps);
        }
        runParticles(screen);
        #else
        fireWorks(screen);
        #endif

        SDL_BlitSurface(menuBg[MENUGFXHELP] , NULL, screen, &(setting()->bgPos) );

        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B_PLAY, HSCREENW, HSCREENH+108);

        if( getButton( C_BTNB ) || isPointerClicked() )
        {
          resetBtn( C_BTNB );

          if(player()->gameStarted) //Return to game if allready started
          {
            startTransition(screen, TRANSITION_TYPE_ROLL_OUT,800);
            return(STATEPLAY);
          } else {
            startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
            setMenu(menuStatePaused);
          }

        }
      break;

      case menuStateAbout:
        starField(screen, 0);
        getInpPointerState()->escEnable=1;
        fireWorks(screen);

        txtWave(screen, FONTSMALL, STR_MENU_ABOUT_WEBSITE, HSCREENW, HSCREENH-120+12,&rot);

        menuMaxY = aboutScreen(screen, &menuPosY, menuChangeY);


        //alt z x z x
        static int cheat=-1;
        if( getButton( C_BTNA ) )
        {
          cheat=0;
          resetBtn( C_BTNA );
        }

        if( (cheat==0 || cheat==2)  && getButton(C_BTNY) )
        {
          resetBtn(C_BTNY);
          cheat++;
        }
        if( (cheat==1 || cheat==3) && getButton(C_BTNX) )
        {
          resetBtn(C_BTNX);
          cheat++;
        }
        if(cheat==4)
        {
          sndPlay( SND_MENUCLICK, 160 );
          cheat=-1;
          stats()->progress = getNumLevels();
        }

        if( getButton(C_BTNB) || isPointerEscapeClicked() || getButton(C_BTNMENU) )
        {
          resetMouseBtn();
          resetBtn( C_BTNB );
          resetBtn( C_BTNMENU );
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
          menuState=menuStatePaused;
        }

      break;

      case menuStateGameOver:

        starField(screen,0);
        txtWriteCenter(screen, FONTMEDIUM,STR_MENU_GAMEOVER, HSCREENW, HSCREENH-85);
        sprintf(buf, STR_MENU_SCORE, player()->campStats.score);
        txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+108);

        //If in arcade mode, lose progress
        if(setting()->arcadeMode)
        {
          stats()->progress=-1;
          statsSave();
        }

        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+60);

        if( getButton(C_BTNB) || isPointerClicked() )
        {
          resetMouseBtn();
          resetBtn( C_BTNB );
          setMenu(menuStatePaused);
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );

          if( statsIsHighScore() )
          {
            setMenu(menuStateEnterHighScore);
            menuReturnHack=menuStatePaused;
          }
        }
      break;

      case menuStateBeatPack:

        starField(screen, 1);
        if( packState()->cp->hasFinishedImg )
        {
          if( !packState()->finishedImg )
          {
            packState()->finishedImg = loadImg( packGetFile("./","finished.png" ) );
            if( !packState()->finishedImg )
            {
              printf("Error: Can't load pack-finished file '%s'\n", packGetFile("./","finished.png" ) );
              return(STATEQUIT);
            }
          }
        } else {
          fireWorks(screen);

          txtWave(screen, FONTMEDIUM, STR_MENU_BEAT_PACK_HEADLINE, HSCREENW, HSCREENH-108, &rot);
          txtWrite(screen, FONTSMALL, STR_MENU_BEAT_PACK_RANT, HSCREENW-150, HSCREENH-50);
        }

        //Blit image to screen
        SDL_BlitSurface(packState()->finishedImg , NULL, screen, &(setting()->bgPos) );

        //bink "press b"
        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+100);

        if( getButton(C_BTNB) || isPointerClicked() )
        {
          resetBtn(C_BTNB);
          resetMouseBtn();

          //Free image
          SDL_FreeSurface( packState()->finishedImg );

          //Set img ptr so it knows to load a new next time.
          packState()->finishedImg=0;

          //Check if he got a highscore.
          if( statsIsHighScore() )
          {
            setMenu(menuStateEnterHighScore);
            menuReturnHack=menuStatePackList;
            menuPosY = packState()->selected;
          } else {
            setMenu(menuStatePackList);
            //Set to first pack in lst when we get there
            menuPosY=0;
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500 );

          }

          return(STATEMENU);
        }
      break;

      case menuStateOutro:
        getInpPointerState()->escEnable=1;

        //Show background image
        if(!menuBg[MENUGFXBYE])
        {
          menuBg[MENUGFXBYE]=loadImg( DATADIR"data/menu/bye.png");
        }
        SDL_BlitSurface(menuBg[MENUGFXBYE] , NULL, screen, &(setting()->bgPos) );
        //Fireworks?
        fireWorks(screen);

        //Blink "Press B"
        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+60);

        if( getButton(C_BTNMENU) || isPointerEscapeClicked() )
        {
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500);
          setMenu(menuStatePaused);
        }

        if(getButton(C_BTNB) || isPointerClicked() )
        {
          resetBtn(C_BTNB);
          cleanUpGame();
          return(STATEQUIT);
        }
      break;

      //The leveleditor submenu.
      case menuStateUserLevels:
        getInpPointerState()->escEnable=1;
        starField(screen, 1);
        fireWorks(screen);

        txtWave(screen, FONTMEDIUM, STR_MENU_LVLEDIT_HEADLINE, HSCREENW, HSCREENH-105, &rot);
        menuMaxY=getNumUserLevels()+1;

        //Just keep it clean. (This is a reference to a pitchshifter song by the way)
        if(player()->gameStarted) cleanUpGame();

        sb = scrollBar(screen, HSCREENW+160-19-15, HSCREENH-120+40, (float)(getNumUserLevels()-1), ((menuPosY<2)?0:menuPosY-2), menuChangeY);
        if(sb != -1)
        {
          menuPosY=sb+2;
        }

        //List levels
        ul=0;   //Userlevel
        scroll=0; //Generic scoll int
        if(menuPosY > 10)
        {
          ul = menuPosY-10;
          scroll=ul;
        }


        int i=0;
        while(ul < getNumUserLevels())
        {
          i++;

          if( menuPosY==ul+2)
          {
            sprintf(buf, STR_MENU_LVLEDIT_USRLVL_SEL, ul);
          } else {
            sprintf(buf, STR_MENU_LVLEDIT_USRLVL, ul);
          }

          txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-53+(11*(ul-scroll)));

          if( isBoxClicked( getTxtBox() ) )
          {
            menuPosY=(ul+2); //hackity hack, if menuposy is negative, we click-selected a level
          }

          ul++;
          if( i > 12 )
          {
            break;
          }
        }

        //Show keys only if cursor not active
        if( !(getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT) )
        {
          txtWriteCenter(screen, FONTSMALL, STR_MENU_LVLEDIT_USAGE, HSCREENW, HSCREENH+106);
        } else  {
          //Show Play Clone Edit buttons for the mouse
          if( menuPosY > 1)
          {

            txtWriteCenter(screen, FONTSMALL, STR_MENU_LVLEDIT_PLAY, HSCREENW-60-48, HSCREENH+106 );
            if( isBoxClicked( getTxtBox() ) )
            {
              resetMouseBtn();
              ul=-1; //Decided that -1 means "play"
             // menuPosY=-menuPosY;
            }

            txtWriteCenter(screen, FONTSMALL, STR_MENU_LVLEDIT_EDIT, HSCREENW-48, HSCREENH+106 );
            if( isBoxClicked( getTxtBox() ) )
            {
              resetMouseBtn();
              ul=-2; //Decided that -2 means "edit"
             // menuPosY=-menuPosY;
            }

            txtWriteCenter(screen, FONTSMALL, STR_MENU_LVLEDIT_CLONE, HSCREENW+60-48, HSCREENH+106 );
            if( isBoxClicked( getTxtBox() ) )
            {
              resetMouseBtn();
              ul=-3; //Decided that -3 means "clone"
            //  menuPosY=-menuPosY;
            }

            txtWriteCenter(screen, FONTSMALL, STR_MENU_LVLEDIT_UPLOAD, HSCREENW+60+24, HSCREENH+106 );
            if( isBoxClicked( getTxtBox() ) )
            {
              resetMouseBtn();
              ul=-4; //Decided that -4 means "Upload"
            //  menuPosY=-menuPosY;
            }

          }
        }


        //Show Create and Exit menu-points.
        if( (!dir || menuPosY!=0)|| (getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT)) txtWriteCenter(screen, FONTSMALL, STR_LVLEDIT_CREATE_CHOICE,HSCREENW,HSCREENH-78);
        if( isBoxClicked( getTxtBox() ) )
        {
          resetMouseBtn();
          menuPosY=0;
          ul=-5; //No special meaning.
        }

        if( (!dir || menuPosY!=1)|| (getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT)) txtWriteCenter(screen, FONTSMALL, STR_LVLEDIT_EXIT_CHOICE,HSCREENW,HSCREENH-65);
        if( isBoxClicked( getTxtBox() ) )
        {
          resetMouseBtn();
          menuPosY=1;
          ul=-5; //No special meaning.
        }

        if( menuPosY > -1 )
        {

          //Edit levels/select
          if(getButton(C_BTNB) || ul < 0 )
          {
            resetBtn(C_BTNB);
            resetMouseBtn();

            if(menuPosY==0) //Load empty, and create new levelname
            {
              //Load empty, set name to something diffrent
              editorLoad( DATADIR"data/empty.wzp",screen);

              sprintf(buf, "%s/level%03i.wzp", getUserLevelDir(), getNumUserLevels());
              editorFileName(buf);

              startTransition(screen, TRANSITION_TYPE_ROLL_OUT, 500 );
              return(STATEEDIT);
            } else if(menuPosY==1)  //Exit from editor
            {
              player()->inEditor=0;
              menuState=menuStatePaused;
              startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
              menuPosY=0;
            } else if( ul==-2 || ul>0 )
            {
              startTransition(screen, TRANSITION_TYPE_ROLL_OUT, 500 );
              editorLoad(userLevelFile(menuPosY-2),screen);
              return(STATEEDIT);
            }
          }

#ifdef PLATFORM_SUPPORTS_STATSUPLOAD
          //Attempt upload
          if( getButton(C_BTNX) || ul==-4 )
          {
            ul=-5;
            resetBtn(C_BTNX);
            resetMouseBtn();

            if( isLevelCompletable(userLevelFile(menuPosY-2)) )
            {
              if( dlcPushFile( userLevelFile(menuPosY-2), message ) )
              {
                menuState=menuStateLevelUploadOk;
              } else {
                menuState=menuStateLevelUploadError;
              }
            } else {
              menuState=menuStateLevelUploadError;
              sprintf(message, "Complete the level first.");
            }
          }
#endif

          //Clone a level
          if( getButton(C_BTNY) || ul==-3 )
          {
            resetBtn(C_BTNY);
            resetMouseBtn();

            if(menuPosY > 1)
            {
              //Load the selected level
              editorLoad(userLevelFile(menuPosY-2),screen);

              //Make new filename
              sprintf(buf, "%s/level%03i.wzp", getUserLevelDir(), getNumUserLevels());
              editorFileName(buf);

              //Start editing state
              startTransition(screen, TRANSITION_TYPE_ROLL_OUT, 500 );

              return(STATEEDIT);
            }
          }

          //Play levelfile
          if( getButton(C_BTNA) || ul==-1 )
          {
            resetBtn(C_BTNA);
            resetMouseBtn();
            if(menuPosY > 1)
            {
              initPlayer(); //Reset player for editor.
              player()->levelFile = userLevelFile(menuPosY-2);
              player()->level = 0;
              player()->inEditor=1;

              if(initGame(screen))
              {
                startTransition(screen, TRANSITION_TYPE_ROLL_OUT, 500 );
                return(STATEPLAY);
              } else {
                printf("Editor couldn't init game for the editor.\n");
                return(STATEQUIT);
              }
            }
          }
        } //Not a negative number


        if(isPointerEscapeClicked())
        {
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
          menuState=menuStatePaused;
          resetMouseBtn();
        }

      break; //< userlevels

      case menuStateLevelUploadOk:
        starField(screen, 1);
        fireWorks(screen);
        getInpPointerState()->escEnable=1;
        txtWriteCenter(screen, FONTMEDIUM, "Thanks!", HSCREENW, HSCREENH-106);

        //TODO: Externalize strings
        txtWrite(screen, FONTSMALL, "Thanks for uploading your level!", HSCREENW-160+8, HSCREENH-78);
        txtWrite(screen, FONTSMALL, "When your level has been reviewed,", HSCREENW-160+8, HSCREENH-78+9);
        txtWrite(screen, FONTSMALL, "it will appear on wizznic.org", HSCREENW-160+8, HSCREENH-78+9+9);
        txtWrite(screen, FONTSMALL, "Then you can get it with the code;", HSCREENW-160+8, HSCREENH-78+9+9+9);
        txtWriteCenter(screen, FONTMEDIUM, message, HSCREENW, HSCREENH-78+9+9+9+9+9);




        if( getButton(C_BTNB) || isPointerEscapeClicked() || getButton(C_BTNMENU) )
        {
          resetMouseBtn();
          resetBtn( C_BTNB );
          resetBtn( C_BTNMENU );
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
          menuState=menuStateUserLevels;
        }

        if(dir && !(getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT )) txtWriteCenter(screen, FONTSMALL, "Press Ctrl", HSCREENW, HSCREENH+60);

        break; //< upload level went ok

      case menuStateLevelUploadError:
        starField(screen, 0);
        getInpPointerState()->escEnable=1;
        txtWriteCenter(screen, FONTMEDIUM, "Sorry!", HSCREENW, HSCREENH-106);
        txtWrite(screen, FONTSMALL, message, HSCREENW-160+28, HSCREENH-78);

        if( getButton(C_BTNB) || isPointerEscapeClicked() || getButton(C_BTNMENU) )
        {
          resetMouseBtn();
          resetBtn( C_BTNB );
          resetBtn( C_BTNMENU );
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
          menuState=menuStateUserLevels;
        }

        if(dir && !(getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT )) txtWriteCenter(screen, FONTSMALL, "Press Ctrl", HSCREENW, HSCREENH+60);

      break; //< something went wrong

      case menuStatePackList:
        starField(screen, 1);
        fireWorks(screen);
        txtWriteCenter(screen, FONTMEDIUM, STR_MENU_SELECT_PACK, HSCREENW, HSCREENH-106);
        getInpPointerState()->escEnable=0;
        if(dir && !(getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT )) txtWriteCenter(screen, FONTSMALL, STR_MENU_SELECT_PACK_PRESS_CTRL, HSCREENW, HSCREENH-84);
        menuMaxY= packState()->numPacks;
        ul=0;
        scroll=0;
        if(menuPosY > 2)
        {
          ul= menuPosY - 2;
          scroll=ul;
        }


        //Clear box
        SDL_FillRect(menuBg[MENUGFXPACKBOX],0, SDL_MapRGB(screen->format, 0,255,255));

        int_fast8_t packListItemWasClicked=0;
        while(ul < packState()->numPacks+1 && ul-scroll <  4)
        {
          //The selected box waves
          if(menuPosY== ul)
          {
            drawPackBox(menuBg[MENUGFXPACKBOX], 0,0, ul );
            setWaving(&waving, screen, menuBg[MENUGFXPACKBOX], HSCREENW-130,HSCREENH-70+(48*(ul-scroll)),2,4,150);
            waveImg(&waving);
          } else {
            drawPackBox(screen, HSCREENW-130,HSCREENH-70+(48*(ul-scroll)), ul );
          }

          //Check if this box was clicked

          r.x = HSCREENW-130;
          r.y = HSCREENH-70+(48*(ul-scroll));
          r.w = r.x+260; // Size of graphics + position I know-, that
          r.h = r.y+42;  // is not how SDL_Rect was intended to be used.

          if( isBoxClicked( &r ) )
          {
            resetMouseBtn();
            if( menuPosY == ul )
            {
              packListItemWasClicked=1;
            } else {
              menuPosY=ul;
            }
          }

          //Increase counter (doh, most obvious comment to date)
          ul++;
        }


        sb = scrollBar(screen, HSCREENW+160-19-5, HSCREENH-120+50, (float)(packState()->numPacks), menuPosY, menuChangeY);
        if(sb != -1) menuPosY=sb;

        if( getButton(C_BTNB) || packListItemWasClicked )
        {
          resetBtn(C_BTNB);
          resetMouseBtn();
          //Check if it's the DLC box
          if( menuPosY == packState()->numPacks )
          {
#if defined( PLATFORM_SUPPORTS_STATSUPLOAD )
            if( setting()->online && dlcGetState()==DLC_READY )
            {
              startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
              setMenu(menuStateDLC);
            }
#endif
          } else {

            packFreeGfx();
            //If it's a different pack
            if(menuPosY != packState()->selected)
            {
              //If there's a game started
              if(player()->gameStarted)
              {
                cleanUpGame();
              }

              //Clear the levelselector image (it caches images)
              resetLevelSelector();

              //Set selected pack.
              packSet(menuPosY);
              printf("Selected pack number: %i\n", menuPosY);

              //Save setting
              free( setting()->packDir);
              setting()->packDir = malloc( sizeof(char)*(strlen(packState()->cp->path)+1) );
              strcpy( setting()->packDir, packState()->cp->path );
              saveSettings(); //Save
              //--

            }
            startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );
            //Return to main menu
            setMenu(menuStatePaused);
          }
        }
      break;

      case menuStateOptions:
        starField(screen, 1);
        fireWorks(screen);
        txtWave(screen, FONTMEDIUM, STR_MENU_OPTIONS, HSCREENW, HSCREENH-108, &rot);

        //Number of options in list
        menuMaxY = 9;

        if(dir || menuPosY!= 0) txtWriteCenter(screen, FONTSMALL, STR_MENU_OPTIONS_EXIT, HSCREENW, HSCREENH-70);
        //Save and exit Options menu
        if( ( getButton( C_BTNMENU ) || (getButton( C_BTNB ) && menuPosY==0) ) || isBoxClicked( getTxtBox() ) )
        {
          resetBtn( C_BTNMENU );
          resetBtn( C_BTNB );
          resetMouseBtn();

          saveSettings();
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500 );

          setMenu(menuStatePaused);
        }

        sprintf(buf, STR_MENU_OPTIONS_SOUNDVOL, setting()->soundVol);
        if(dir || menuPosY!= 1) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-50);
        if( isBoxClicked( getTxtBox() ) )
        {
          resetMouseBtn();
          menuPosY=1;
          menuMaxX=16;
          menuPosX = setting()->soundVol/8;
          incPosX();
        }

        //Set posY
        if(menuPosY==1)
        {
          if(menuChangeY)
          {
            menuMaxX=16;
            menuPosX = setting()->soundVol/8;
          }
          setting()->soundVol = menuPosX*8;

          //Change sound-volume if it changed
          if(menuChangeX)
          {
            applySettings();
          }
        }

        sprintf(buf, STR_MENU_OPTIONS_MUSICVOL, setting()->musicVol);
        if(dir || menuPosY!= 2) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-40);
        if( isBoxClicked( getTxtBox() ) )
        {
          menuPosY=2;
          menuMaxX=16;
          menuPosX = setting()->musicVol/8;
          incPosX();
        }

        //Set posY
        if(menuPosY==2)
        {
          if(menuChangeY)
          {
            menuMaxX=16;
            menuPosX = setting()->musicVol/8;
          }
          setting()->musicVol = menuPosX*8;
          //Change music-volume if it changed
          if(menuChangeX)
          {
            applySettings();
          }
        }

        sprintf(buf, (setting()->userMusic)?"Music: <User selected>":"Music: <Game Music>");
        if(dir || menuPosY!=3) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW,HSCREENH-30);
        if(  (menuPosY==3  && getButton(C_BTNB)) || isBoxClicked( getTxtBox() ) )
        {
          resetBtn(C_BTNB);
          menuPosY=0;
          fileListMake(setting()->musicDir);
          setMenu(menuStateSelectMusicDir);
        }

        sprintf(buf, (setting()->arcadeMode)?"Game mode: < Arcade >":"Game mode: < Career >");
        if(dir || menuPosY!= 4) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-10);
        if( isBoxClicked( getTxtBox() ) )
        {
          menuPosY=4;
          menuMaxX=1;
          menuPosX = setting()->arcadeMode;
          incPosX();
        }

        //Set posy
        if(menuPosY==4)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->arcadeMode;
          }
          setting()->arcadeMode = menuPosX;
        }

        sprintf(buf, (setting()->showFps)?"Show FPS: < On >":"Show FPS: < Off >");
        if(dir || menuPosY!= 5) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+10);
        if( isBoxClicked( getTxtBox() ) )
        {
          menuPosY=5;
          menuMaxX=1;
          menuPosX = setting()->showFps;
          incPosX();
        }

        //Set posy
        if(menuPosY==5)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->showFps;
          }
          setting()->showFps = menuPosX;
        }

        sprintf(buf, (setting()->particles)?"Particles: < On >":"Particles: < Off >");
        if(dir || menuPosY!= 6) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+20);
        if( isBoxClicked( getTxtBox() ) )
        {
          menuPosY=6;
          menuMaxX=1;
          menuPosX = setting()->particles;
          incPosX();
        }
        //Set posy
        if(menuPosY==6)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->particles;
          }
          setting()->particles = menuPosX;
        }
        //Disable music (requires restart)
        sprintf(buf, (setting()->disableMusic)?"Load Music: < Disabled >":"Load Music: < Enabled >");
        if(dir || menuPosY!= 7) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+30);
        if( isBoxClicked( getTxtBox() ) )
        {
          menuPosY=7;
          menuMaxX=1;
          menuPosX = setting()->disableMusic;
          incPosX();
        }
        //Set posy
        if(menuPosY==7)
        {
          txtWriteCenter(screen, FONTSMALL, "(Restart required)", HSCREENW, HSCREENH+100);
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->disableMusic;
          }
          setting()->disableMusic = menuPosX;
        }

        #ifdef WIZ // to do for GP2X
        sprintf(buf, "Wiz Clock: < %i >", setting()->wizClock);
        if(dir || menuPosY!= 8) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+50);
        if( isBoxClicked( getTxtBox() ) )
        {
          menuReturnHack=menuStateOptions;
          setMenu(menuStateNoPointerSupport);
        }

        //Set posY
        if(menuPosY==8)
        {
          if(menuChangeY)
          {
            menuMaxX=70;
            menuPosX = setting()->wizClock/10;
          }

          if( menuPosX < 10 )
            menuPosX = 10;

          setting()->wizClock = menuPosX*10;

          //Only clock the wiz when value changes.
          if(menuChangeX)
          {
            WIZ_SetClock( setting()->wizClock );
          }
        }
        #endif


        //Upload stats?
        #ifdef PLATFORM_SUPPORTS_STATSUPLOAD
        sprintf(buf, (setting()->uploadStats)?"Allow Online: <Yes>":"Allow Online: <No>");
        if(dir || menuPosY!= 8) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+50);
        if( isBoxClicked( getTxtBox() ) )
        {
          menuPosY=8;
          menuMaxX=1;
          menuPosX = setting()->uploadStats;
          incPosX();
        }

        //Set posY
        if(menuPosY==8)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX=setting()->uploadStats;
          }
          setting()->uploadStats = menuPosX;
          if(menuChangeX)
          {
            if(setting()->uploadStats && !setting()->online)
            {
              statsUpload(0,0,0,0,0,"check",1, &(setting()->session) );
              dlcCheckOnline();
            } else if(!setting()->uploadStats)
            {
              setting()->online=0;
            }
          }
        }
        #endif

        //Reset progress?
        if( stats()->progress != -1 || 1)
        {
          //Disable music (requires restart)

          if(dir || menuPosY!= 9) txtWriteCenter(screen, FONTSMALL, "Clear Progress and High-Scores", HSCREENW, HSCREENH+70);
          if(  (menuPosY==9  && getButton(C_BTNB)) || isBoxClicked( getTxtBox() ) )
          {
            resetBtn(C_BTNB);
            setMenu(menuStateConfirmReset);
            menuPosX = 1;
          }
        }



      break;

      //Highscore list
      case menuStateHighScores:
      starField(screen, 0);
      fireWorks(screen);
      menuMaxY=1;

      statsDrawHs(screen);


      txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B,HSCREENW,HSCREENH+100);

      if( getButton( C_BTNB ) || isPointerClicked() )
      {
        resetBtn( C_BTNB );
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500);
          setMenu(menuStatePaused);
      }

      break;

      case menuStateEnterHighScore:
        starField(screen, 0);
        INP_GET_STR_LIMITS
        txtWriteCenter(screen, FONTMEDIUM, "High score!", HSCREENW,HSCREENH-95);
        txtWriteCenter(screen, FONTSMALL, "Enter your name for highscore.", HSCREENW, HSCREENH-70);

        if( inpStrGet(strStateHighScore, menuPosX, menuPosY, dir) )
        {
          strcpy( setting()->playerName, strStateHighScore->str );

          saveSettings();
          //Copy player name.
          strcpy(player()->campStats.name, setting()->playerName);
          statsSaveHighScore();
         // printf("AddingHighscore: %i\n", player()->campStats.score);
          startTransition(screen, TRANSITION_TYPE_CURTAIN_DOWN, 500);
          menuState=menuReturnHack;
          menuPosY=0;
        }
      break;

      case menuStateSelectMusicDir:

        starField(screen,0);
        txtWave(screen, FONTMEDIUM, "Select Music", 160,15, &rot);
        if(dir || menuPosY!=0) txtWriteCenter(screen, FONTSMALL, "Play Music From Game", HSCREENW, HSCREENH-70);
        if( isBoxClicked( getTxtBox() ) || (menuPosY==0 && getButton(C_BTNB) ) )
        {
          resetBtn(C_BTNB);
          setting()->userMusic=0; //Turn user-selected music off
          free(setting()->musicDir);
          setting()->musicDir=malloc(sizeof(char)*2 );
          strcpy(setting()->musicDir, "."); //This forces musicdir to be set to pwd next load, which is fine if user-music is not used anyway.
          saveSettings();
          fileListFree();
          soundSetMusic(); //Important to set after freeing file list, since this is using that list
          setMenu(menuStateOptions);
          menuPosX=3;
          return(STATEMENU);
        }

        if(dir || menuPosY!=1) txtWriteCenter(screen, FONTSMALL, "Play Music From Here", HSCREENW, HSCREENH-60);

        if( isBoxClicked( getTxtBox() ) || (menuPosY==1 && getButton(C_BTNB)) )
        {
          resetBtn(C_BTNB);

          {
            setting()->userMusic=1; //Turn user-selected music on
            saveSettings();
            fileListFree();
            soundSetMusic(); //Important to set after freeing file list, since this is using that list
            setMenu(menuStateOptions);
            menuPosX=3;
            return(STATEMENU);
          }
        }

        x=0;
        y=0;
        scroll=0;
        if(menuPosY > 10)
        {
          x = menuPosY-10;
          scroll=x;
        }
        //Run through list
        it=&fileList()->begin;

        while( LISTFWD(fileList(), it) )
        {
          fItem=(fileListItem_t*)it->data;
          if(fItem->dir)
          {
            y++;
            if(y>x)
            {
              if(dir || menuPosY!=x+2) txtWriteCenter(screen, FONTSMALL, fItem->name, HSCREENW, (HSCREENH-40)+((x-scroll)*10));
              if( (menuPosY-2==x && getButton(C_BTNB)) || isBoxClicked( getTxtBox() ) )
              {
                resetBtn(C_BTNB);

                if( fItem->fullName == (char*)0 ) //The "Go back" entry.
                {
                  char* nmd = malloc( sizeof(char)*(strlen(setting()->musicDir)+1) ); //New directory name.
                  #ifdef WIN32
                    #define DELIMCHAR '\\'
                  #else
                    #define DELIMCHAR '/'
                  #endif
                  nmd = strncpy( nmd, setting()->musicDir, charrpos( setting()->musicDir, DELIMCHAR ) );
                  nmd[charrpos( setting()->musicDir, DELIMCHAR )] = 0;
                  printf("Went from '%s' to '%s'\n", setting()->musicDir, nmd);
                  free( setting()->musicDir );
                  setting()->musicDir = nmd;
                } else {
                  free(setting()->musicDir);
                  setting()->musicDir=malloc(sizeof(char)*(strlen(fItem->fullName)+1));
                  strcpy(setting()->musicDir, fItem->fullName);
                }
                fileListMake(setting()->musicDir);
                menuPosY=2;
                return(STATEMENU);
              }
              x++;
            }
          }
        }

        txtWriteCenter(screen, FONTSMALL, "----- Files -----", HSCREENW, (HSCREENH-40)+(x-scroll)*10);
        x++;
        menuMaxY=x;

        it=&fileList()->begin;
        while( LISTFWD(fileList(), it) )
        {
          fItem=(fileListItem_t*)it->data;
          if(!fItem->dir)
          {
            if(dir || menuPosY!=x+2) txtWriteCenter(screen, FONTSMALL, fItem->name, HSCREENW, (HSCREENH-40)+(x-scroll)*10);
            x++;
          }
        }

      break;

      #if !defined (GP2X) || !defined (WIZ)
      case menuStateUploadDiag:
        starField(screen,0);
        if( menuYesNo == 0)
        {
          menuYesNo = mkAni(loadImg( DATADIR"data/menu/yesno.png"), 36,42,0);
        }

        r.y = (setting()->bgPos.y)+ 240-64-16;

        menuPosY=0; //Using this as a click-lock
        menuMaxY=1;
        menuMaxX=2;
        txtWrite(screen, FONTSMALL, STR_MENU_UPLOADNAG, HSCREENW-152, HSCREENH-108 );


        //Set rect for yes
        r.x = (setting()->bgPos.x)+ 32;
        //Display/blink yes
        if(menuPosX == 0 && dir)
        {
          //SDL_BlitSurface(menuBg[MENUGFXYES] , NULL, screen, &ynpos );
          drawSprite(screen, menuYesNo->spr[0], r.x, r.y);
        } else {
          drawSprite(screen, menuYesNo->spr[1], r.x, r.y);
        }
        //Detect pointer click on yes
        r.w=r.x + menuYesNo->spr[0]->clip.w;
        r.h=r.y + menuYesNo->spr[0]->clip.h;
        if( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT &&  isPointerInBox( &r ) )
        {
          menuPosX=0;
          menuPosY=1; //Slightly obscure.. :p
        }

        if( menuPosX == 1 && dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_ALLOW_UPLOAD_U, HSCREENW, HSCREENH+100-32-16 );
        txtWriteCenter(screen, FONTSMALL, STR_MENU_ALLOW_UPLOAD, HSCREENW, HSCREENH+100-32-16);
        if( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT && isPointerInBox( getTxtBox() ) )
        {
          menuPosX=1;
        }

        //Set rect for no
        r.x = (setting()->bgPos.x)+ 320-32-menuYesNo->spr[0]->clip.w;
        //Display/blink no
        if(menuPosX == 2 && dir)
        {
         // SDL_BlitSurface(menuBg[MENUGFXNO] , NULL, screen, &ynpos );
          drawSprite(screen, menuYesNo->spr[2], r.x, r.y);
        } else {
          drawSprite(screen, menuYesNo->spr[3], r.x, r.y);
        }
        //Detect pointer click on no
        r.w=r.x + menuYesNo->spr[0]->clip.w;
        r.h=r.y + menuYesNo->spr[0]->clip.h;
        if( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT && isPointerInBox( &r ) )
        {
          menuPosX=2;
          menuPosY=1; //Slightly obscure.. :p
        }


        if( getButton( C_BTNB) || (isPointerClicked() && menuPosY==1) )
        {
          resetBtn( C_BTNB );
          resetMouseBtn();
          //Enabled
          if( menuPosX == 0 )
          {
            setting()->uploadStats=1;
            statsUpload(0,0,0,0,0,"check",1, &(setting()->session));

            #ifdef PLATFORM_SUPPORTS_STATSUPLOAD
            dlcCheckOnline();
            #endif
          }

          //Disabled
          if( menuPosX == 2 )
          {
            setting()->uploadStats=0;
            sndPlay(SND_LOSER, 160);
          } else {
            sndPlay(SND_WINNER, 160);
          }

          if( (menuPosX == 0 || menuPosX == 2) )
          {
            SDL_FreeSurface(menuYesNo->spr[0]->img);
            freeAni(menuYesNo);
            menuYesNo=0;

            setting()->firstRun=0;
            saveSettings();
            startTransition(screen, TRANSITION_TYPE_CURTAIN_UP, 500);
            setMenu( menuStatePaused );
          }
        }
      break;

      case menuStateUpdate:
        starField(screen,1);
        fireWorks(screen);
        menuMaxY=0;
        menuMaxX=2;
        txtWrite(screen, FONTSMALL, STR_MENU_UPDATE, HSCREENW-152, HSCREENH-50 );
        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+70);
        if( getButton( C_BTNB) || isPointerClicked() )
        {
          resetBtn( C_BTNB );
          resetMouseBtn();
          menuState=menuStatePaused;
        }
      break;
      #endif

      case menuStateNoPointerSupport:
        starField(screen,0);
        menuMaxY=0;
        menuMaxX=0;
        txtWrite(screen, FONTSMALL, STR_MENU_NOPTRSUPPORT, HSCREENW-152, HSCREENH-58 );
        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+70);
        if( isPointerClicked() )
        {
          resetMouseBtn();
          menuState=menuStatePaused;
        } else if( getButton( C_BTNB) )
        {
          resetBtn( C_BTNB );
          menuState=menuReturnHack;
        }
      break;

      case menuStateConfirmReset:
        starField(screen,0);

        menuPosY=0; //Using this as a click-lock
        menuMaxY=1;
        menuMaxX=2;
        txtWriteCenter(screen, FONTMEDIUM, STR_MENU_ARE_YOU_SURE, HSCREENW,HSCREENH-95-16);
        sprintf(buf, STR_MENU_CONFIRM_CLEAR_HIGHSCORES, packState()->cp->name);
        txtWrite(screen, FONTSMALL, buf, HSCREENW-152, HSCREENH-70-16 );


        if( menuYesNo == 0)
        {
          menuYesNo = mkAni(loadImg( DATADIR"data/menu/yesno.png"), 36,42,0);
        }

        r.y = (setting()->bgPos.y)+ 240-60;

        //Set rect for yes
        r.x = (setting()->bgPos.x)+ 32;
        //Display/blink yes
        if(menuPosX == 0 && dir)
        {
          //SDL_BlitSurface(menuBg[MENUGFXYES] , NULL, screen, &ynpos );
          drawSprite(screen, menuYesNo->spr[0], r.x, r.y);
        } else {
          drawSprite(screen, menuYesNo->spr[1], r.x, r.y);
        }
        //Detect pointer click on yes
        r.w=r.x + menuYesNo->spr[0]->clip.w;
        r.h=r.y + menuYesNo->spr[0]->clip.h;
        if( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT &&  isPointerInBox( &r ) )
        {
          menuPosX=0;
          menuPosY=1; //Slightly obscure.. :p
        }

        if( menuPosX == 1 && dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_ALLOW_RESET_U, HSCREENW, HSCREENH+100-25 );
        txtWriteCenter(screen, FONTSMALL, STR_MENU_ALLOW_RESET, HSCREENW, HSCREENH+100-25);
        if( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT && isPointerInBox( getTxtBox() ) )
        {
          menuPosX=1;
        }

        //Set rect for no
        r.x = (setting()->bgPos.x)+ 320-32-menuYesNo->spr[0]->clip.w;
        //Display/blink no
        if(menuPosX == 2 && dir)
        {
         // SDL_BlitSurface(menuBg[MENUGFXNO] , NULL, screen, &ynpos );
          drawSprite(screen, menuYesNo->spr[2], r.x, r.y);
        } else {
          drawSprite(screen, menuYesNo->spr[3], r.x, r.y);
        }
        //Detect pointer click on no
        r.w=r.x + menuYesNo->spr[0]->clip.w;
        r.h=r.y + menuYesNo->spr[0]->clip.h;
        if( getInpPointerState()->timeSinceMoved < POINTER_SHOW_TIMEOUT && isPointerInBox( &r ) )
        {
          menuPosX=2;
          menuPosY=1; //Slightly obscure.. :p
        }


        if( getButton( C_BTNB) || (isPointerClicked() && menuPosY==1) )
        {
          SDL_FreeSurface(menuYesNo->spr[0]->img);
          freeAni(menuYesNo);
          menuYesNo=0;

          resetBtn(C_BTNB);
          resetMouseBtn();

          if( menuPosX==0)
          {
            statsReset();
            setMenu(menuStateOptions);
          }

          if( menuPosX==2)
          {
            setMenu(menuStateOptions);
          }
        }
      break;

      #if defined( PLATFORM_SUPPORTS_STATSUPLOAD )
      case menuStateDLC:
        starField(screen,1);
        INP_GET_STR_LIMITS
        getInpPointerState()->escEnable=0;

        if( dlcGetState() == DLC_BUSY )
        {
          fireWorks(screen);
          txtWriteCenter(screen, FONTMEDIUM, "Installing..", HSCREENW,HSCREENH-95);
          txtWriteCenter(screen, FONTSMALL, "Please wait while installing pack.", HSCREENW, HSCREENH-70);
        } else if( dlcGetState() == DLC_INSTALLED )
        {
          fireWorks(screen);
          txtWriteCenter(screen, FONTMEDIUM, "Success!", HSCREENW,HSCREENH-95);
          txtWriteCenter(screen, FONTSMALL, "It's ready to play!", HSCREENW, HSCREENH-60);

          if( getChar() == SDLK_RETURN || getButton(C_BTNMENU) || getButton(C_BTNB) || isPointerEscapeClicked() || isPointerClicked() )
          {
            resetBtn(C_BTNMENU);
            resetBtn(C_BTNB);
            setMenu( menuStatePackList );
            menuPosY = packAdd( bundlePath(), PACK_IS_DLC );
            bundlePathReset();
            dlcSetReady();
          }
        } else if( dlcGetState() == DLC_READY )
        {
          getInpPointerState()->escEnable=1;

          txtWriteCenter(screen, FONTMEDIUM, "Install Puzzles!", HSCREENW,HSCREENH-95);
          txtWriteCenter(screen, FONTSMALL, "Please enter the DLC Code", HSCREENW, HSCREENH-70);


          if( getButton(C_BTNMENU) || isPointerEscapeClicked() )
          {
            setMenu(menuStatePackList);
            resetBtn(C_BTNB);
          }

          if( inpStrGet(strStateDlcCode, menuPosX, menuPosY, dir ) )
          {
            dlcTryInstall(strStateDlcCode->str, getUsrPackDir() );
          }

        } else if( dlcGetState() == DLC_FAILED )
        {
          txtWriteCenter(screen, FONTMEDIUM, "Error", HSCREENW,HSCREENH-95);
          txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_DOWNLOAD, HSCREENW-150, HSCREENH-70);

          if( getChar() == SDLK_RETURN || getButton(C_BTNMENU) || getButton(C_BTNB) || isPointerEscapeClicked() || isPointerClicked() )
          {
            resetMouseBtn();
            dlcSetReady();
            setMenu(menuStateDLC);
          }

        } else if( dlcGetState() == DLC_API_OUTDATED )
        {
          txtWriteCenter(screen, FONTMEDIUM, "Update Wizznic", HSCREENW,HSCREENH-95);
          txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_API_OUTDATED, HSCREENW-150, HSCREENH-70);
          if( getChar() == SDLK_RETURN || getButton(C_BTNMENU) || getButton(C_BTNB) || isPointerEscapeClicked() || isPointerClicked() )
          {
            resetMouseBtn();
            setMenu(menuStatePackList);
          }
        } else if( dlcGetState() == DLC_BUNDLE_ERROR )
        {
          txtWriteCenter(screen, FONTMEDIUM, "Install Error", HSCREENW,HSCREENH-95);
          switch( getBundleError() )
          {
            case BUNDLE_FAIL_CORRUPT:
              txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_CORRUPT, HSCREENW-150, HSCREENH-70);
              break;
            case BUNDLE_FAIL_COULD_NOT_OPEN:
              txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_COULD_NOT_OPEN, HSCREENW-150, HSCREENH-70);
              break;
            case BUNDLE_FAIL_DIR_EXISTS:
              txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_DIR_EXISTS, HSCREENW-150, HSCREENH-70);
              break;
            case BUNDLE_FAIL_NOT_BUNDLEFILE:
              txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_NOT_BUNDLEFILE, HSCREENW-150, HSCREENH-70);
              break;
            case BUNDLE_FAIL_NO_WRITE_PERMISSION:
              txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_NO_WRITE_PERMISSION, HSCREENW-150, HSCREENH-70);
              break;
            case BUNDLE_FAIL_UNSUPPORTED_VERSION:
              txtWrite(screen, FONTSMALL, STR_MENU_DLC_ERROR_UNSUPPORTED_VERSION, HSCREENW-150, HSCREENH-70);
              break;
          }
          if( getChar() == SDLK_RETURN || getButton(C_BTNMENU) || getButton(C_BTNB) || isPointerEscapeClicked() || isPointerClicked() )
          {
            resetMouseBtn();
            dlcSetReady();
            setMenu(menuStatePackList);
          }
        }


      break;
      #endif
  }
  menuChangeX=0;
  menuChangeY=0;

  return(STATEMENU);
}
