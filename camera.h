#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL.h>

typedef struct {
	float x, y, w, h;
	int winW, winH;
} CAM;

extern CAM cameraPos;

void updateCamera(float x, float y);
void cameraSetWindowSize(int w, int h);
SDL_FRect worldEntity2Screen(float x, float y, float w, float h);
#endif
