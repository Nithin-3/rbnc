#include "args.h"
#include "camera.h"
#include "hud.h"
#include "ws.h"
#include "object.h"
#include "input.h"
#include "render.h"
#include "world.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>

#define FPS 60.0

int main(int argc, char *argv[]) {
	parseArgs(argc, argv);
	wsInit();
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("rbnc", 0, 0, SDL_WINDOW_FULLSCREEN);
	if (!window) {
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!SDL_SetRenderVSync(renderer, -1))
		SDL_SetRenderVSync(renderer, 1);
	if (!renderer) {
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	hud_init(renderer);
	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		cameraSetWindowSize(w, h);
	}

	if (!wsWaitForInit()) {
		SDL_Log("WebSocket init failed");
		running = 0;
	}

	Uint64 prev, freq;
	freq = SDL_GetPerformanceFrequency();
	prev = SDL_GetPerformanceCounter();  // get nano second cuz it never overflow
					     // SDL_GetTicks() (ms) overflow after ~45 days in 32 bit pc maybe
	static double hudAccum, accumulator;
	hudAccum = accumulator = 0.0;
	while (running) {
		Uint64 now = SDL_GetPerformanceCounter();
		dt = (double)(now - prev) / freq;
		accumulator += dt,
		    hudAccum += dt;
		prev = now;

		// NOTE: runs at exactly fixed(FPS) times per second
		// not even time gap
		while (accumulator >= dtFix) {
			handleInput();
			accumulator -= dtFix;
		}
		if (hudAccum >= 0.1) {	// 10fps
			hud_update(renderer, dt > 0 ? (int)(1.0 / dt) : 0);
			hudAccum = 0.0;
		}

		wsPoll();					 // poll for WebSocket
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	 // set world color
		SDL_RenderClear(renderer);
		render(window, renderer);
		hud_render(renderer);
		SDL_RenderPresent(renderer);
	}

	circle_cache_destroy();
	hud_destroy();
	wsStop();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;


	// FIXME: 
	// turn off vsync 
	// set fixed target frame rate 120 fps
	// target all devive render in fixed fps 
	//
}
