

#include "input.h"

uint8_t running = 1;

void handleInput() {
	SDL_Event e;
	Uint64 prev = SDL_GetTicks();
	while (SDL_PollEvent(&e)) {  //  event driven, fires once when something happens
		if (e.type == SDL_EVENT_QUIT)
			running = 0;
		if (e.type == SDL_EVENT_WINDOW_RESIZED)
			cameraSetWindowSize(e.window.data1, e.window.data2);
		if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_SPACE)
			toggleDraw();
	}

	const bool *keys = SDL_GetKeyboardState(NULL);	// fires EVERY frame while key is held

	uint8_t dir = 0;
	if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
		dir |= DIR_UP;
	if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
		dir |= DIR_DOWN;
	if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
		dir |= DIR_LEFT;
	if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
		dir |= DIR_RIGHT;
	updatePlayer(dir);
	sendPeriodicPing();

	// FIX:
	// make a prediction of playes
	// update to rbnc stack
}
