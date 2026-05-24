#include "player.h"
#include "camera.h"
#include "world.h"

void initPlayer(float x, float y, uint32_t color) {
	Player p = {
		.x = x,
		.y = y,
		.r = 30,
		.color = color,
	};
	insertPlayer(p);
	updateCamera(p.x, p.y);
}
