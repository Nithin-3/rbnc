

#include "input.h"
#include "hashMap.h"
#include "player.h"
#include "world.h"
#include "ws.h"
#include <string.h>

uint8_t running = 1;
static uint8_t draw = 0;
static uint32_t color = 0;
uint32_t seq = 0, currentFrame = 0;

char msg[msgSize];

void handleInput() {
	if (!color) {
		color = playerColor();
		return;
	}
	SDL_Event e;
	while (SDL_PollEvent(&e)) {  //  event driven, fires once when something happens
		if (e.type == SDL_EVENT_QUIT)
			running = 0;
		if (e.type == SDL_EVENT_WINDOW_RESIZED)
			cameraSetWindowSize(e.window.data1, e.window.data2);
		if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_SPACE)
			draw = !draw;
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

	// bit flags: UP=1, DOWN=2, LEFT=4, RIGHT=8
	// 0000 1111 (15) all direction are pressed
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

	// send > {type dir color seq}
	msg[0] = draw;
	msg[1] = dir;
	memcpy(msg + (2 * sizeof(uint8_t)), &color, sizeof(color));
	memcpy(msg + (2 * sizeof(uint8_t)) + sizeof(uint32_t), &seq, sizeof(seq));
	seq++;

	sendMsg(&msg, msgSize);
	sendPeriodicPing();

	// dtFixms = dtFix * 1000
	// how long it run = dtFixms * seq + gametime(ms)
	// current frame = (how long it run) / dtFixms
	//
	// generate state seq --> current frame
	//
	// check diff < FRAME_LEN
	// %FRAME_LEN give date order from to  in real store data index
	// default start with dir = draw = 0
	// generate frame for all player

	double dtFixms = dtFix * 1000;
	currentFrame = (dtFixms * seq + gameTime) / dtFixms;

	if (currentFrame - seq > FRAME_LEN)
		return;

	for (int i = 0; i < PLAYER_LEN; i++) {
		if (history[i].key == EMPTY_KEY)
			continue;  // skip empty slots

		HASH *entry = &history[i];
		for (int f = seq - 1; f < currentFrame; f++) {
			entry->val[f % FRAME_LEN] = entry->val[(f + 1) % FRAME_LEN];
		}

		STATE pridicted = entry->val[currentFrame % FRAME_LEN];
		if (pridicted.draw) {
			Entity e = { .x = pridicted.x, .y = pridicted.y, .color = entry->key };
			insertEntity(e);
		}

		for (int i = 0; i < PLAYER_LEN; i++) { // is wasting the cpu
			if (player[i] == NULL)
				continue;
			Player *p = player[i];
			if (entry->key == p->color) {
				p->x = pridicted.x;
				p->y = pridicted.y;
			}
		}
	}
}
