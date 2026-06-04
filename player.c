#include "camera.h"
#include "player.h"
#include "world.h"
#include "ws.h"
#include <string.h>

static uint8_t draw = 0, plIndx = -1;
static uint32_t seq = 0;

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

void updatePlayer(uint8_t dir) {     // bit flags: UP=1, DOWN=2, LEFT=4, RIGHT=8
	if (plIndx < 0 || dir > 15)  // 0000 1111 (15) all direction are pressed
				     // 0 -> no movement
				     // 1 -> up
				     // 2 -> down
				     // 3 -> up + down
				     // 4 -> left
				     // 5 -> left + up
				     // 6 -> left + down
				     // 7 -> left + up + down
				     // 8 -> right
				     // 9 -> right + up
				     // 10 -> right + down
				     // 11 -> right + up + down
				     // 12 -> right + left
				     // 13 -> right + left + up
				     // 14 -> right + left + down
				     // 15 -> right + left + up + down
		return;

	playerEvent evt = {
		.dir = dir,
		.color = player[plIndx]->color,
		.type = draw,
		.seq = seq++,
	};
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
