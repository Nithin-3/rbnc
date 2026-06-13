
#include "world.h"
#include "hashMap.h"
#include <stdint.h>
#include <stdlib.h>

HASH history[PLAYER_LEN];

void historyInit(void) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		history[i].key = EMPTY_KEY;
	}
}

static uint8_t hash(uint32_t key) {  // 0 <= hash < PLAYER_LEN
	key = ((key >> 16) ^ key) * 0x45d9f3b;
	key = ((key >> 16) ^ key) * 0x45d9f3b;
	key = (key >> 16) ^ key;
	return key & (PLAYER_LEN - 1);
}

int hashSet(uint32_t key, STATE state, int stateIdx) {
	uint8_t idx = hash(key);
	for (int i = 0; i < PLAYER_LEN; i++) {
		uint8_t slot = (idx + i) & (PLAYER_LEN - 1);
		if (history[slot].key == key) {
			history[slot].val[stateIdx] = state;
			return -2;
		}
		if (history[slot].key == EMPTY_KEY) {
			history[slot].key = key;
			history[slot].val[stateIdx] = state;
			return 0;
		}
	}
	return -1;
}

HASH *hashGet(uint32_t key) {
	uint8_t idx = hash(key);
	for (int i = 0; i < PLAYER_LEN; i++) {
		uint8_t slot = (idx + i) & (PLAYER_LEN - 1);
		if (history[slot].key == key)
			return &history[slot];
		if (history[slot].key == EMPTY_KEY)
			return NULL;
	}
	return NULL;
}
