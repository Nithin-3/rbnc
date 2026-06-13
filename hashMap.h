#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include "world.h"

#define FRAME_LEN 10
#define EMPTY_KEY 0xFF

typedef struct {
	float x, y;
	uint8_t draw, done;
} STATE;

extern uint8_t historyKeys[PLAYER_LEN];
extern STATE history[PLAYER_LEN][FRAME_LEN];

void historyInit(void);
int hashSet(uint32_t key, STATE state, int stateIdx);
int hashGet(uint32_t key);

#endif
