#include "hud.h"
#include "world.h"
#include <stdio.h>
#include <inttypes.h>

#define HUD_TEX_W 300
#define HUD_TEX_H 200

static SDL_Texture *hudTex = NULL;

void hud_init(SDL_Renderer *renderer) {
	hudTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, HUD_TEX_W, HUD_TEX_H);
	SDL_SetTextureBlendMode(hudTex, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, hudTex);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);
}

void hud_update(SDL_Renderer *renderer, int fps) {
	if (!hudTex) return;
	SDL_SetRenderTarget(renderer, hudTex);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

	char line[128];
	int y = 0;

	snprintf(line, sizeof(line), "FPS: %d", fps);
	SDL_RenderDebugText(renderer, 0, y, line);
	y += 10;

	snprintf(line, sizeof(line), "ping: %" PRIu64 "ms", ping);
	SDL_RenderDebugText(renderer, 0, y, line);
	y += 10;

	for (int i = 0; i < PLAYER_LEN; i++) {
		if (player[i] == NULL) continue;
		snprintf(line, sizeof(line), "%s: %.0f, %.0f", player[i]->name, player[i]->x, player[i]->y);
		SDL_RenderDebugText(renderer, 0, y, line);
		y += 10;
	}

	SDL_SetRenderTarget(renderer, NULL);
}

void hud_render(SDL_Renderer *renderer) {
	if (!hudTex) return;
	SDL_FRect dst = { 0, 0, HUD_TEX_W, HUD_TEX_H };
	SDL_RenderTexture(renderer, hudTex, NULL, &dst);
}

void hud_destroy(void) {
	if (hudTex) {
		SDL_DestroyTexture(hudTex);
		hudTex = NULL;
	}
}
