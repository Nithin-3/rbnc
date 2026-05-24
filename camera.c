
#include "camera.h"
#include <stdio.h>

#define width 500
#define height 300
CAM cameraPos = { '\0', '\0', '\0', '\0' };

void updateCamera(float x, float y) {
	cameraPos.x = x - (width * 0.5);
	cameraPos.y = y - (height * 0.5);
	cameraPos.w = width;
	cameraPos.h = height;
}

SDL_FRect worldEntity2Screen(SDL_Window *win, float x, float y, float w, float h) {
	SDL_FRect r = { -1, -1, -1, -1 };
	if (cameraPos.x == '\0' || cameraPos.y == '\0' || cameraPos.w == '\0' || cameraPos.h == '\0')
		return r;
	int wid, hei;
	if (!SDL_GetWindowSize(win, &wid, &hei)) {
		printf("SDL_GetCurrentDisplayMode Error: %s\n", SDL_GetError());
		SDL_Quit();
		return r;
	}
	float sX = (float)wid / width, sY = (float)hei / height;
	float scale = sX > sY ? sY : sX;

	r.x = (x - cameraPos.x) * scale;
	r.y = (y - cameraPos.y) * scale;
	r.w = w * scale;
	r.h = h * scale;
	return r;
}
