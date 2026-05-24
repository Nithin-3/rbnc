#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL.h>

typedef struct {
	float x, y, w, h;
} CAM;

extern CAM cameraPos;

void updateCamera(float x, float y);
SDL_FRect worldEntity2Screen(SDL_Window *win, float x, float y, float w,float h);
#endif
