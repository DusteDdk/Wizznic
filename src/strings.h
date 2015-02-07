#ifndef STRINGS_H_INCLUDED
#define STRINGS_H_INCLUDED

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

//Specific for platform
#if defined (GP2X) || defined (WIZ) ///gp2x and Wiz version
  //Menu
  #define STR_MENU_PRESS_B            "Press B"
  #define STR_MENU_PRESS_B_PLAY       "Press B to Play"

  #define STR_MENU_LVLEDIT_USAGE      "Edit: B. Play: A. Clone: Y"

  //Editor
  #define STR_EDIT_CONTROLS "X:Put Y:Del Menu:Exit Select:Save"

  //Entering highscores
  #define STR_STRINPUT_CONTROLS "B:Type  A:Delete "

  //Game
  #define STR_GAME_RESTARTCONFIRM     "Press A to confirm"
  #define STR_GAME_PRESSB             "Press B to Play"
  #define STR_SELECT_CTRLS            "Use D-PAD, button B.\n"


#elif defined (PSP) /// PSP version
  //Menu
  #define STR_MENU_PRESS_B            "Press Cross"
  #define STR_MENU_PRESS_B_PLAY       "Press Cross to Play"

  #define STR_MENU_LVLEDIT_USAGE      "Edit: X. Play: O. Clone: Triangle"

  //Editor
  #define STR_EDIT_CONTROLS "Squ:Put Tri:Del Start:Exit Sel:Save"

  //Entering highscores
  #define STR_STRINPUT_CONTROLS  "Start:Save  X:Type  O:Delete"

  //Game
  #define STR_GAME_RESTARTCONFIRM     "Press Circle to confirm"
  #define STR_GAME_PRESSB             "Press Cross to Play"

  #define STR_SELECT_CTRLS           "Use D-PAD, Cross.\n"

#elif defined (PANDORA)
    //Menu
    #define STR_MENU_PRESS_B            "Press B"
    #define STR_MENU_PRESS_B_PLAY       "Press B to Play"



    #define STR_MENU_LVLEDIT_USAGE      "Edit: B. Play: A. Clone: Y"

    //Editor
    #define STR_EDIT_CONTROLS "X:Put Y:Del START:Exit SELECT:Save"

    //Entering highscores
    #define STR_STRINPUT_CONTROLS  "B:Type  A:Delete"

    //Game
    #define STR_GAME_RESTARTCONFIRM     "Press A to confirm"
    #define STR_GAME_PRESSB             "Press B to Play"
    #define STR_SELECT_CTRLS            "Use D-PAD, button B.\n"
#elif defined(GCW0) /// GCW Zero version
  // Note: GCW Zero button layout matches the GP2X/Wiz layout in the following manner:
  //  GCW Zero - GP2X/Wiz
  //  A        - B
  //  B        - X
  //  X        - A
  //  Y        - Y
  //  L        - L
  //  R        - R
  //  START    - MENU
  //  SELECT   - SELECT

  //Menu
  #define STR_MENU_PRESS_B            "Press A"
  #define STR_MENU_PRESS_B_PLAY       "Press A to Play"

  #define STR_MENU_LVLEDIT_USAGE      "Edit: A. Play: X. Clone: Y"

  //Editor
  #define STR_EDIT_CONTROLS "B:Put Y:Del Start:Exit Select:Save"

  //Entering highscores
  #define STR_STRINPUT_CONTROLS "A:Type  B:Delete "

  //Game
  #define STR_GAME_RESTARTCONFIRM     "Press again to confirm"
  #define STR_GAME_PRESSB             "Press A to Play"
  #define STR_SELECT_CTRLS            "Use D-PAD, button A.\n"


#elif defined(MAME_CTRL)
    //Menu
    #define STR_MENU_PRESS_B            "Press Button 1"
    #define STR_MENU_PRESS_B_PLAY       "Press B1 to Play"

    #define STR_MENU_LVLEDIT_USAGE      "Edit: B4. Play: B3. Clone: B2"

    //Editor
    #define STR_EDIT_CONTROLS "1:Put 2:Del s1:Exit s2:Save"

    //Entering highscores
    #define STR_STRINPUT_CONTROLS  "B1:Type  B2:Delete"

    //Game
    #define STR_GAME_RESTARTCONFIRM     "Press B2 to confirm"
    #define STR_GAME_PRESSB             "Press B1 to Play"
    #define STR_SELECT_CTRLS            "Use Joystick, button 1.\n"
 #else
    //Menu
    #define STR_MENU_PRESS_B            "Press Ctrl"
    #define STR_MENU_PRESS_B_PLAY       "Press Ctrl to Play"
    #define STR_SELECT_CTRLS            "Use Arrows,Ctrl or Mouse.\n"

    #define STR_MENU_LVLEDIT_USAGE      "Edit: Ctrl. Play: Alt. Clone: Z"

    //Editor
    #define STR_EDIT_CONTROLS "X:Put Z:Del ESC:Exit Space:Save"

    //Entering highscores
    #define STR_STRINPUT_CONTROLS  "Ctrl:Type  Alt:Delete"

    //Game
    #define STR_GAME_RESTARTCONFIRM     "Again to confirm"
    #define STR_GAME_PRESSB             "Press Ctrl to Play"
#endif


///General strings
#define CODE_VERSION "1.0-dev"

#ifdef BUILD_NUMBER
  #ifdef RELEASE_VERSION
    #define VERSION_STRING RELEASE_VERSION"(B"BUILD_NUMBER")"
  #else
    #define VERSION_STRING CODE_VERSION"(B"BUILD_NUMBER")"
  #endif
#else
  #define VERSION_STRING CODE_VERSION
#endif

#ifdef WITH_OPENGL
  #define STR_VID_OPTIONS STR_VID_OPTIONS_OPENGL""STR_VID_OPTIONS_SW
#else
  #define STR_VID_OPTIONS STR_VID_OPTIONS_SW
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
#define STR_MENU_PACK_CHOICE        "Packs"
#define STR_MENU_EXIT_CHOICE        "Exit"
#define STR_MENU_PACK_SHOW          "Pack: %s"

#define STR_MENU_PACKLIST_DLC_ENTER "Get More Packs!\n"\
		                                "Enter a DLC code here,\n"\
		                                "to install more puzzles!"

#define STR_MENU_PACKLIST_DLC_OFFLINE "Wizznic is offline.\n"\
		                                  "DLC download is disabled.\n"\
                                      "Go to wizznic.org/dlc"

#define STR_MENU_ABOUT_WEBSITE      "http://wizznic.org/"

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

#define STR_MENU_UPLOADNAG          "-- Wizznic loves The Internet! --\n"\
                                    "\n"\
                                    "  Internet access lets Wizznic\n"\
                                    "  fetch DLCs, check for new\n"\
                                    "  versions  and send back\n"\
                                    "  some game-play info to\n"\
                                    "  make the game better.\n"\
                                    "\n"\
                                    "\n"\
                                    STR_SELECT_CTRLS

#define STR_MENU_ALLOW_UPLOAD        "<- Allow Online ->"
#define STR_MENU_ALLOW_UPLOAD_U     "<__________________>"



#define STR_MENU_ALLOW_RESET        "Delete         Back"
#define STR_MENU_ALLOW_RESET_U      "_________or________"


//                                  "----------------------------------"
#define STR_MENU_UPDATE             "---- Wizznic has been updated ----\n"\
                                    "Great news everyone!\n"\
                                    "There's a new version out!\n"\
                                    "Grab it at:\n"\
                                    "http://wizznic.org/\n"\
                                    "I'm sure it's filled with some\n"\
                                    "pretty cool and awesome stuff! :)\n"\
                                    "----------------------------------\n"


//                                  "----------------------------------"
#define STR_MENU_NOPTRSUPPORT       "-- Pointer not supported here --\n"\
                                    "You need to use the cursor keys\n"\
                                    "to do this, because I am\n"\
                                    "too lazy to make the pointer work\n"\
                                    "here.. Press key to continue,\n"\
                                    "or click to return to the menu.\n"\
                                    "If you consider donating a local\n"\
                                    "beer, I will consider making\n"\
                                    "the pointer work here.. - Jimmy."


//                                        "----------------------------------"
#define STR_MENU_DLC_ERROR_DOWNLOAD       "I'm sorry!\n\n"\
		                                      "For some reason it was not\n"\
                                          "possible to download a DLC using\n"\
                                          "the code you entered...\n"\
                                          "Maybe the code was wrong?\n"\
                                          "Maybe Wizznic could not connect\n"\
                                          "to the server? It could be down..\n\n"\
                                          "Try again please!\n\n"\
                                          "If you can not get it to work:\n"\
                                          "Visit http://wizznic.org/\n"



//                                        "----------------------------------\n"
#define STR_MENU_DLC_ERROR_API_OUTDATED   "This version of Wizznic! is a bit\n"\
		                                      "too dusty..\n"\
		                                      "\n"\
		                                      "It's too old for DLCs. :(\n"\
		                                      "Please update Wizznic!"


//                                        "----------------------------------\n"
#define STR_MENU_DLC_ERROR_CORRUPT        "The DLC file was corrupt.\n"\
		                                      "\n"\
		                                      "Please try again."


//                                        "----------------------------------\n"
#define STR_MENU_DLC_ERROR_COULD_NOT_OPEN "Could not open the DLC file,\n"\
		                                      "please check the permissions of:\n"\
                                          "%s\n"\
                                          "And try again try again."


//                                        "----------------------------------\n"
#define STR_MENU_DLC_ERROR_DIR_EXISTS     "This DLC is already intalled.\n"\
		                                      "If it's not working, please go to\n"\
		                                      "your DLC directory and\n"\
		                                      "delete it and try again."


//                                        "----------------------------------\n"
#define STR_MENU_DLC_ERROR_NOT_BUNDLEFILE "The downloaded file is not\n"\
		                                      "recognized as a Wizznic DLC."


//                                              "----------------------------------\n"
#define STR_MENU_DLC_ERROR_NO_WRITE_PERMISSION  "Wizznic did not have permission\n"\
		                                            "to install the DLC."

//                                              "---- Wizznic has been updated ----\n"
#define STR_MENU_DLC_ERROR_UNSUPPORTED_VERSION  "This version of Wizznic do not\n"\
		                                            "know how to install that DLC,\n"\
		                                            "maybe it is too old ?"\

#define STR_MENU_GAMEOVER           "Game Over!"
#define STR_MENU_SCORE              "Score %i"

#define STR_MENU_LVLEDIT_HEADLINE   "Level Editor!"

//Editor
#define STR_LVLEDIT_MORE            "[More]"
#define STR_LVLEDIT_CREATE_CHOICE   "Create"
#define STR_LVLEDIT_EXIT_CHOICE     "Exit"
#define STR_MENU_LVLEDIT_USRLVL     "Level %i"
#define STR_MENU_LVLEDIT_USRLVL_SEL "--> Level %i <--"

#define STR_MENU_LVLEDIT_PLAY       "Play"
#define STR_MENU_LVLEDIT_CLONE      "Clone"
#define STR_MENU_LVLEDIT_EDIT       "Edit"

#define STR_EDIT_SAVED              "Saved"
#define STR_EDIT_UNSAVED            "Not saved"
#define STR_EDIT_STATUS             "Status:"
#define STR_EDIT_NOT_SAVED_WARNING  "Not Saved!"


#if defined(GP2X) || defined(WIZ)
  #define STR_EDIT_NEXTBRICK_KEY      "B>"
  #define STR_EDIT_PREVBRICK_KEY      "<A"
#elif defined (PSP)
  #define STR_EDIT_NEXTBRICK_KEY      "X>"
  #define STR_EDIT_PREVBRICK_KEY      "<Y"
#elif defined (PANDORA)
  #define STR_EDIT_NEXTBRICK_KEY      "B>"
  #define STR_EDIT_PREVBRICK_KEY      "<A"
#elif defined (GCW0)
  #define STR_EDIT_NEXTBRICK_KEY      "A>"
  #define STR_EDIT_PREVBRICK_KEY      "<X"
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

//Confirm clear highscore
#define STR_MENU_ARE_YOU_SURE       "Are You Sure ?"
#define STR_MENU_CONFIRM_CLEAR_HIGHSCORES "\n--- This will be deleted ---\n"\
                                          "\nFrom: %s\n\n"\
                                          "  * High-Scores\n"\
                                          "  * Statistics\n"\
                                          "  * Unlocked levels\n"\
                                          "  * Your current game\n"\
                                          "\n"\
                                          "Are you sure you want to do this?"

//Packs
#define STR_MENU_SELECT_PACK        "Select pack"
#define STR_MENU_SELECT_PACK_PRESS_CTRL STR_MENU_PRESS_B" to select"
#define STR_MENU_PACKS_MORE         "[More]"

//Game
#define STR_GAME_RESTARTWARNING     "Restart Level?"
#define STR_GAME_UNSOLVABLE         "Unsolvable!"
#define STR_GAME_LOSTLIFE           "Life Lost!"
#define STR_GAME_OUTOFTIME          "Out Of Time!"


//This put to stdout
#define STR_VID_OPTIONS_OPENGL \
    "    -gl           # Enable OpenGL.\n"\
    "    -sw           # Disable OpenGL.\n"\
    "    -glheight PX  # Window width  (-1 for auto).\n"\
    "    -glwidth  PY  # Window height (-1 for auto).\n"\
    "    -glfilter ST  # 0=Blocky, 1=Smooth.\n"\
    "    -rift         # Usable for Oculus Rift.\n"

#define STR_VID_OPTIONS_SW \
    "    -f            # Enable fullscreen.\n"\
    "    -w            # Disable fullscreen.\n"\
    "    -z 2          # Software scale to 640x480.\n"\
    "    -d PACKNAME   # Dump screenshots of levels.\n\n"




//Well, string functions...
void stripNewLine(char* str);
int splitVals(char ch,const char* buf, char* set, char* val); //Splits a setting=value line and returns true if it did. else returns 0
int charrpos(const char* str, char c); //Return position of last instance of character c

#endif // STRINGS_H_INCLUDED
