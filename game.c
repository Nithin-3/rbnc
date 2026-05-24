#include "world.h"
#include "render.h"
#include "player.h"
#include <SDL3/SDL.h>

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("rbnc", 0, 0, SDL_WINDOW_FULLSCREEN);
	if (!window) {
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	initPlayer(3, 3, 0xFFFFFFFF);

	Uint64 prev = SDL_GetTicks();
	SDL_Event e;
	int running = 1;
	while (running) {
		Uint64 now = SDL_GetTicks();
		dt = (now - prev) / 1000.0f;
		prev = now;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT)
				running = 0;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		render(window, renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
