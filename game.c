#include "args.h"
#include "camera.h"
#include "ws.h"
#include "object.h"
#include "player.h"
#include "render.h"
#include "world.h"
#include <SDL3/SDL.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	parseArgs(argc, argv);
	wsInit();
	wsServiceLoop();
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("rbnc", 0, 0, SDL_WINDOW_FULLSCREEN);
	if (!window) {
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!SDL_SetRenderVSync(renderer, -1)) {
		SDL_SetRenderVSync(renderer, 1);
	}
	if (!renderer) {
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		cameraSetWindowSize(w, h);
	}

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
			if (e.type == SDL_EVENT_WINDOW_RESIZED) {
				cameraSetWindowSize(e.window.data1, e.window.data2);
			}
			if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_SPACE) {
				toggleDraw();
			}
		}

		const bool *keys = SDL_GetKeyboardState(NULL);
		float dirX = 0, dirY = 0;

		if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
			dirY -= 1;
		if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
			dirY += 1;
		if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
			dirX -= 1;
		if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
			dirX += 1;
		updatePlayer(dirX, dirY);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		render(window, renderer);
		SDL_RenderPresent(renderer);

		// float fps = 1.0f / dt;
		//
		// fprintf(stdout,"\rFPS:\t%.2f", fps);
	}

	circle_cache_destroy();
	wsStop();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
