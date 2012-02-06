#ifndef POINTER_H_INCLUDED
#define POINTER_H_INCLUDED

#include <SDL/SDL.h>

typedef struct {
  int startX, startY; //position it started to be pressed on the board
  int curX, curY;     //Position on the board (11x11 positions)
  int vpX, vpY;       // "virtual"/scaled pixel positions (320x230)ish
  int downTime;       //Milliseconds curser have been pressed down, is 0 first iteration
  int timeSinceMoved; //Milliseconds since cursor last moved, is 0 first iteration
  int isDown;         //Is 1 if the pointer is pressed down
  int_fast8_t hitABox; //Set0 each click, set1 if isBoxClicked returns true
  int_fast8_t escEnable;// If 1, the "escape" box is shown in the upper corner
  uint32_t colWhite;   //Color of dots in cursor
} inpPointerState_t;

inline inpPointerState_t* getInpPointerState();

int_fast8_t isPointerClicked(); //This does not take into account if the pointer actually hit anything
int_fast8_t isAnyBoxHit(); //This will return true if any of the calls to isBoxClicked did this tick.
int_fast8_t isBoxClicked( SDL_Rect* r );

int_fast8_t isPointerEscapeClicked(); //Returns 1 if the "escbox" was hit and escEnable=1;

void initPointer(SDL_Surface* screen);
inline void drawPointer(SDL_Surface* screen);

#endif // POINTER_H_INCLUDED
