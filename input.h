#ifndef INPUT_H
#define INPUT_H

#include "camera.h"
#include "player.h"
#include <SDL3/SDL_events.h>

#define msgSize (2 * sizeof(uint8_t) + 2 * sizeof(uint32_t))
#define DIR_UP (1)		// 0001
#define DIR_DOWN (1 << 1)	// 0010
#define DIR_LEFT (1 << 2) 	// 0100
#define DIR_RIGHT (1 << 3)	// 1000

extern uint8_t running;
void handleInput();

#endif
