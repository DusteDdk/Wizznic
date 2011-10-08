#ifndef PIXEL_H_INCLUDED
#define PIXEL_H_INCLUDED

#include <SDL/SDL.h>

inline void plotPixel(SDL_Surface* img, int x, int y, uint32_t col);
inline void plotPixelu(SDL_Surface* img, int x, int y, uint16_t col);
inline uint32_t freadPixel(SDL_Surface* img, int x, int y);

#endif // PIXEL_H_INCLUDED
