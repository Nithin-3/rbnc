#ifndef OBJECT_H
#define OBJECT_H

#include <SDL3/SDL.h>
#include <stdint.h>

struct center {
	float x, y;
};

void circle(SDL_Renderer *renderer, struct center c, int radius, int innerfill, int thickness, uint32_t color);

#endif
