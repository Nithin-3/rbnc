
#include "camera.h"

#define width 640
#define height 360
CAM cameraPos = { '\0', '\0', '\0', '\0', 0, 0 };

void updateCamera(float x, float y) {
	cameraPos.x = x - (width * 0.5);
	cameraPos.y = y - (height * 0.5);
	cameraPos.w = width;
	cameraPos.h = height;
}

void cameraSetWindowSize(int w, int h) {
	cameraPos.winW = w;
	cameraPos.winH = h;
}

SDL_FRect worldEntity2Screen(float x, float y, float w, float h) {
	SDL_FRect r = { -1, -1, -1, -1 };
	if (cameraPos.x == '\0' || cameraPos.y == '\0' || cameraPos.w == '\0' || cameraPos.h == '\0')
		return r;
	float sX = (float)cameraPos.winW / width, sY = (float)cameraPos.winH / height;
	float scale = sX > sY ? sY : sX;

	r.x = (x - cameraPos.x) * scale;
	r.y = (y - cameraPos.y) * scale;
	r.w = w * scale;
	r.h = h * scale;
	return r;
}
