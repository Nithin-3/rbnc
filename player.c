#include "camera.h"
#include "player.h"
#include "world.h"
#include "ws.h"
#include <math.h>
#include <string.h>
#define speed 50

static int plIndx = -1;
static uint8_t draw = 0;

void initPlayer(float x, float y, uint32_t color, const char name[64]) {
	static int pl = 0;
	if (pl)
		return;
	Player p = {
		.x = x,
		.y = y,
		.r = 100,
		.color = color,
	};
	if (name)
		strncpy(p.name, name, sizeof(p.name) - 1);
	plIndx = insertPlayer(&p);
	if (plIndx < 0)
		return;
	updateCamera(p.x, p.y);
	pl = 1;
}

void updatePlayer(float dirX, float dirY) {
	if (plIndx < 0 || dirX < -1 || dirX > 1 || dirY < -1 || dirY > 1)
		return;
	float len = sqrt(dirX * dirX + dirY * dirY);
	if (len <= 0)
		return;
	float x = player[plIndx]->x + (dirX / len) * speed * dt,
	      y = player[plIndx]->y + (dirY / len) * speed * dt;

	playerEvent evt = {
		.x = x,
		.y = y,
		.color = player[plIndx]->color,
		.type = draw,
	};
	if (!awaitingPing) {
		sendTime = SDL_GetTicks();
		awaitingPing = 1;
	}
	sendMsg(&evt, sizeof(evt));
}

uint8_t toggleDraw() {
	return draw = !draw;
}

uint32_t playerColor() {
	return plIndx < 0 || plIndx > PLAYER_LEN ? 0 : player[plIndx]->color;
}

static uint64_t lastPingTime = 0;

void sendPeriodicPing(void) {
	if (plIndx < 0)
		return;
	uint64_t now = SDL_GetTicks();
	if (now - lastPingTime < 2000)
		return;
	lastPingTime = now;

	if (!awaitingPing) {
		sendTime = SDL_GetTicks();
		awaitingPing = 1;
	}
	unsigned char buff[1 + sizeof(uint32_t)];
	buff[0] = 0x02;
	memcpy(buff + 1, &player[plIndx]->color, sizeof(player[plIndx]->color));
	sendMsg(&buff, sizeof(buff));
}
