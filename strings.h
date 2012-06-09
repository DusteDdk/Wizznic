#ifndef STRINGS_H_INCLUDED
#define STRINGS_H_INCLUDED


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
 * along with Wizznic.  If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

//Specific for platform
#if defined (GP2X) || defined (WIZ) ///gp2x and Wiz version
  //Menu
  #define STR_MENU_PRESS_B            "Press B"
  #define STR_MENU_PRESS_B_PLAY       "Press B to Play"

  #define STR_MENU_LVLEDIT_USAGE      "Edit: B. Play: A. Clone: Y"

  //Editor
  #define STR_EDIT_CONTROLS "X:Put Y:Del Menu:Exit Select:Save"

  //Entering highscores
  #define STR_MENU_HIGHSCORE_NAME_CONTROLS  "B:Type  A:Delete "

  //Game
  #define STR_GAME_RESTARTCONFIRM     "Press A to confirm"
  #define STR_GAME_PRESSB             "Press B to Play"

#elif defined (PSP) /// PSP version
  //Menu
  #define STR_MENU_PRESS_B            "Press Cross"
  #define STR_MENU_PRESS_B_PLAY       "Press Cross to Play"

  #define STR_MENU_LVLEDIT_USAGE      "Edit: X. Play: O. Clone: Triangle"

  //Editor
  #define STR_EDIT_CONTROLS "Squ:Put Tri:Del Start:Exit Sel:Save"

  //Entering highscores
  #define STR_MENU_HIGHSCORE_NAME_CONTROLS  "Start:Save  X:Type  O:Delete"

  //Game
  #define STR_GAME_RESTARTCONFIRM     "Press Circle to confirm"
  #define STR_GAME_PRESSB             "Press Cross to Play"


#else ///Pc version
  #ifdef MAME_CTRL ///Mame
    //Menu
    #define STR_MENU_PRESS_B            "Press Button 1"
    #define STR_MENU_PRESS_B_PLAY       "Press B1 to Play"

    #define STR_MENU_LVLEDIT_USAGE      "Edit: B4. Play: B3. Clone: B2"

    //Editor
    #define STR_EDIT_CONTROLS "1:Put 2:Del s1:Exit s2:Save"

    //Entering highscores
    #define STR_MENU_HIGHSCORE_NAME_CONTROLS  "B1:Type  B2:Delete"

    //Game
    #define STR_GAME_RESTARTCONFIRM     "Press B2 to confirm"
    #define STR_GAME_PRESSB             "Press B1 to Play"

  #else
    /// added by farox for pandora port
    #ifdef PANDORA
    //Menu
    #define STR_MENU_PRESS_B            "Press B"
    #define STR_MENU_PRESS_B_PLAY       "Press B to Play"

    #define STR_MENU_LVLEDIT_USAGE      "Edit: B. Play: A. Clone: Y"

    //Editor
    #define STR_EDIT_CONTROLS "X:Put Y:Del START:Exit SELECT:Save"

    //Entering highscores
    #define STR_MENU_HIGHSCORE_NAME_CONTROLS  "B:Type  A:Delete"

    //Game
    #define STR_GAME_RESTARTCONFIRM     "Press A to confirm"
    #define STR_GAME_PRESSB             "Press B to Play"
    ///////////////////////////////////////////////////////////////
    #else ///Normal
    //Menu
    #define STR_MENU_PRESS_B            "Press Ctrl"
    #define STR_MENU_PRESS_B_PLAY       "Press Ctrl to Play"

    #define STR_MENU_LVLEDIT_USAGE      "Edit: Ctrl. Play: Alt. Clone: Z"

    //Editor
    #define STR_EDIT_CONTROLS "X:Put Z:Del ESC:Exit Space:Save"

    //Entering highscores
    #define STR_MENU_HIGHSCORE_NAME_CONTROLS  "Ctrl:Type  Alt:Delete"

    //Game
    #define STR_GAME_RESTARTCONFIRM     "Press Alt to confirm"
    #define STR_GAME_PRESSB             "Press Ctrl to Play"
    #endif
  #endif

#endif


///General strings

#ifdef BUILD_NUMBER
  #define VERSION_STRING "v1.0-dev (build "BUILD_NUMBER")"
#else
  #define VERSION_STRING "v1.0-dev"
#endif

//Menu
#define STR_MENU_PUZZLES_WORLDWIDE  "Puzzles worldwide: %i"
#define STR_MENU_NEW_GAME           "New Game!"

#define STR_MENU_PAUSE_HEADLINE     "Pause!"
#define STR_MENU_WIZZNIC_HEADLINE   "Wizznic!"

#define STR_MENU_NEW_GAME_CHOICE    "New Game"
#define STR_MENU_RESUME_CHOICE      "Continue"
#define STR_MENU_HIGHSCORES_CHOICE  "Highscores"
#define STR_MENU_OPTIONS_CHOICE     "Options"
#define STR_MENU_EDITOR_CHOICE      "Editor"
#define STR_MENU_ABOUT_CHOICE       "About"
#define STR_MENU_HELP_CHOICE        "Help"
#define STR_MENU_EXIT_CHOICE        "Exit"
#define STR_MENU_PACK_CHOICE        "Pack: %s"

#define STR_MENU_ABOUT_HEADLINE     "About Wizznic!"
#define STR_MENU_ABOUT_TEXT         "Wizznic is an arcade game based on\n"\
                                    "the Taito classic \"Puzznic\" (1989)\n"\
                                    "\n"\
                                    "Wizznic was written in C with SDL.\n"\
                                    "Sfx: See data/media-licenses.txt\n"\
                                    "Gfx/Code: Jimmy Christensen\n"\
                                    "It is Free OpenSource Software\n"\
                                    "licensed under the GPLv3.\n\n"\
                                    "If you like this game, please\n"\
                                    "show your appreciation\n"\
                                    "by creating levels and/or\n"\
                                    "sound-effects or gfx themes!\n"\
                                    ".... or donate something :P"
#define STR_MENU_ABOUT_WEBSITE      "http://wizznic.sf.net/"

#define STR_MENU_BEAT_PACK_HEADLINE "Good job!"
#define STR_MENU_BEAT_PACK_RANT     "Congratulations!\n"\
                                    "You've beat this pack! Now go try\n"\
                                    "out some of the other packs!\n"\
                                    "\n"\
                                    "or maybe you should try and make\n"\
                                    "a pack or some levels yourself??\n"\
                                    "\n"\
                                    "You could maybe make a donation?\n"\
                                    "I'd love to have som funds for\n"\
                                    "Advertisement.. and beer.. ;)\n"\
                                    "But mostly bee... Advertisements!\n"\
                                    "                        - Jimmy"

#define STR_MENU_UPLOADNAG          "------- Information Stuffs -------\n"\
                                    "Wizznic! is a free game that needs\n"\
                                    "your help to become really great!\n"\
                                    "There is no crackteam of puzzle\n"\
                                    "gods paid to make it awesome..\n"\
                                    "\n"\
                                    "Please allow Wizznic! to upload\n"\
                                    "statistics about the gameplay.\n"\
                                    "It's anonymous, secure, free and\n"\
                                    "you won't even notice it's there!\n"\
                                    "\n"\
                                    "\n"\
                                    "Use arrow keys to select, and\n"\
                                    "CTRL to choose, please help :)\n"\
                                    "----------------------------------\n"

#define STR_MENU_UPDATE             "---- Wizznic has been updated ----\n"\
                                    "Great news everyone!\n"\
                                    "There's a new version available!\n"\
                                    "Grab it at your leisure at:\n"\
                                    "http://sf.net/projects/wizznic/\n"\
                                    "I'm sure it's filled with some\n"\
                                    "Pretty cool and awesome stuff! :)\n"\
                                    "----------------------------------\n"

#define STR_MENU_NOPTRSUPPORT       "-- Pointer not supported here --\n"\
                                    "You need to use the cursor keys\n"\
                                    "to do this, because I am\n"\
                                    "too lazy to make the pointer work\n"\
                                    "here.. Press key to continue,\n"\
                                    "or click to return to the menu.\n"\
                                    "If you consider donating a local\n"\
                                    "beer, I will consider making\n"\
                                    "the pointer work here.. - Jimmy."



#define STR_MENU_GAMEOVER           "Game Over!"
#define STR_MENU_SCORE              "Score %i"

#define STR_MENU_LVLEDIT_HEADLINE   "Level Editor!"

//Editor
#define STR_LVLEDIT_MORE            "[More]"
#define STR_LVLEDIT_CREATE_CHOICE   "Create"
#define STR_LVLEDIT_EXIT_CHOICE     "Exit"
#define STR_MENU_LVLEDIT_USRLVL     "Level %i"

#define STR_EDIT_SAVED              "Saved"
#define STR_EDIT_UNSAVED            "Not saved"
#define STR_EDIT_STATUS             "Status:"
#define STR_EDIT_NOT_SAVED_WARNING  "Not Saved!"


#ifdef GP2X
  #define STR_EDIT_NEXTBRICK_KEY      "B>"
  #define STR_EDIT_PREVBRICK_KEY      "<A"
#elif defined (PSP)
  #define STR_EDIT_NEXTBRICK_KEY      "X>"
  #define STR_EDIT_PREVBRICK_KEY      "<Y"
#elif defined (PANDORA)
  #define STR_EDIT_NEXTBRICK_KEY      "B>"
  #define STR_EDIT_PREVBRICK_KEY      "<A"
#else
  #define STR_EDIT_NEXTBRICK_KEY      "Alt"
  #define STR_EDIT_PREVBRICK_KEY      "Ctrl"
#endif

//This is defined in strings.c
extern const char* str_brick_names[];

#ifdef PANDORA /// added by farox for pandora port
#define STR_EDIT_PRESS_EXIT_TO_EXIT "Press START to exit without saving."
#define STR_EDIT_PRESS_SAVE_TO_SAVE "Press SELECT to save the level."
#else
#define STR_EDIT_PRESS_EXIT_TO_EXIT "Press Exit to exit without saving."
#define STR_EDIT_PRESS_SAVE_TO_SAVE "Press Save to save the level."
#endif

#define STR_EDIT_BRICK              "Brick"

//Options
#define STR_MENU_OPTIONS            "Options!"
#define STR_MENU_OPTIONS_EXIT       "Exit Options"
#define STR_MENU_OPTIONS_SOUNDVOL   "Effects Volume: < %i >"
#define STR_MENU_OPTIONS_MUSICVOL   "Music Volume: < %i >"


//Packs
#define STR_MENU_SELECT_PACK        "Select pack"
#define STR_MENU_SELECT_PACK_PRESS_CTRL STR_MENU_PRESS_B" to select"
#define STR_MENU_PACKS_MORE         "[More]"

//Game
#define STR_GAME_RESTARTWARNING     "Restart Level?"
#define STR_GAME_UNSOLVABLE         "Unsolvable!"
#define STR_GAME_OUTOFTIME          "Out Of Time!"

//Well
void stripNewLine(char* str);
int splitVals(char ch,const char* buf, char* set, char* val); //Splits a setting=value line and returns true if it did. else returns 0
int charrpos(const char* str, char c); //Return position of last instance of character c

#endif // STRINGS_H_INCLUDED
