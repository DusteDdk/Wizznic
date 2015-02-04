#ifndef SWITCH_H_INCLUDED
#define SWITCH_H_INCLUDED

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

#include "teleport.h"
#include "board.h"

typedef struct telePort_s switch_t;

int switchSetTargets( playField* pf );

int switchFindTele( playField* pf, int x, int y );
int switchIsValidTarget( playField* pf, int x, int y );
void switchAttachTarget( playField* pf, switch_t* sw );
int switchAmIEnabled(playField* pf, int x, int y);

void switchUpdateAll( playField* pf );


void switchAffectTarget( playField* pf, int x, int y, int newState );

void switchPutBack( playField* pf);

#endif // SWITCH_H_INCLUDED
