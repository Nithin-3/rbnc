#ifndef OBJECT_H
#define OBJECT_H

#include <SDL3/SDL.h>
#include <stdint.h>

struct center {
	float x, y;
};

#define CIRCLE_CACHE_SIZE 10

void circle_player(SDL_Renderer *renderer, struct center c, int radius, uint32_t color);
void circle_entity(SDL_Renderer *renderer, struct center c, int radius, uint32_t color);
void circle_cache_destroy(void);

#endif
