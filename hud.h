#ifndef HUD_H
#define HUD_H

#include <SDL3/SDL.h>

void hud_init(SDL_Renderer *renderer);
void hud_update(SDL_Renderer *renderer, int fps);
void hud_render(SDL_Renderer *renderer);
void hud_destroy(void);

#endif
