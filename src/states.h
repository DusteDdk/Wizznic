#ifndef STATES_H_INCLUDED
#define STATES_H_INCLUDED

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

#define STATEPLAY 0
#define STATEMENU 1
#define STATEEDIT 2
#define STATEQUIT 3

#define menuStateIntro 0
#define menuStateNewGame 1
#define menuStateNextLevel 2
#define menuStateHowto 3
#define menuStateGameOver 4
#define menuStateAbout 5
#define menuStatePaused 6
#define menuStateFinishedLevel 7
#define menuStateBeatPack 9
#define menuStateOutro 10

#define menuStateUserLevels 11

#define menuStatePackList 12
#define menuStateOptions 13
#define menuStateHighScores 14

#define menuStateEnterHighScore 15

#define menuStateSelectMusicDir 16

#define menuStateUploadDiag 17

#define menuStateUpdate 18

#define menuStateNoPointerSupport 19

#define menuStatePrepareNextLevel 20

#define menuStateConfirmReset 21

#define menuStateDLC 22

#define menuStateLevelUploadOk 23

#define menuStateLevelUploadError 24

#endif // STATES_H_INCLUDED
