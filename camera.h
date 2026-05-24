#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL.h>

typedef struct {
	float x, y, w, h;
} CAM;

extern CAM cameraPos;

void updateCamera(int x, int y);
SDL_FRect worldEntity2Screen(SDL_Window *win, int x, int y, int w, int h);
#endif
