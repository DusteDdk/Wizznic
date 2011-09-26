#ifndef PIXEL_H_INCLUDED
#define PIXEL_H_INCLUDED

#include <SDL/SDL.h>

inline void plotPixel(SDL_Surface* img, int x, int y, uint16_t col);
inline void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col);
inline uint16_t freadPixel(SDL_Surface* img, int x, int y);
inline void readPixel(SDL_Surface* img, int x, int y, int* r, int* g, int* b);

#endif // PIXEL_H_INCLUDED
