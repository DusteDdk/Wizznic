#ifndef SWITCH_H_INCLUDED
#define SWITCH_H_INCLUDED

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
