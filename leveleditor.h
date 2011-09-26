#ifndef LEVELEDITOR_H_INCLUDED
#define LEVELEDITOR_H_INCLUDED

#include <SDL/SDL.h>


void editorCleanUp();
void editorLoad(const char* fileName);
void editorFileName(const char* fileName);
int runEditor(SDL_Surface* screen);

#endif // LEVELEDITOR_H_INCLUDED
