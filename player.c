#include "camera.h"
#include "player.h"
#include "world.h"
#include "ws.h"
#include <math.h>
#define speed 100

static int plIndx = -1;

void initPlayer(float x, float y, uint32_t color) {
	static int pl = 0;
	if (pl)
		return;
	Player p = {
		.x = x,
		.y = y,
		.r = 100,
		.color = color,
	};
	insertPlayer(&p);
	plIndx = p.index;
	updateCamera(p.x, p.y);
	pl = 1;
}

void updatePlayer(float dirX, float dirY) {
	if (plIndx < 0 || dirX < -1 || dirX > 1 || dirY < -1 || dirY > 1)
		return;
	float len = sqrt(dirX * dirX + dirY * dirY);
	if (len <= 0)
		return;
	player[plIndx]->x += (dirX / len) * speed * dt,
	    player[plIndx]->y += (dirY / len) * speed * dt;

	playerEvent evt = {
		.x = player[plIndx]->x,
		.y = player[plIndx]->y,
		.color = player[plIndx]->color,
		.type = 0,
	};
	sendMsg(&evt);
	updateCamera(player[plIndx]->x, player[plIndx]->y);
}
