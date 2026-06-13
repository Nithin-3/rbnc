
#include "world.h"
#include "hashMap.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t historyKeys[PLAYER_LEN];
STATE history[PLAYER_LEN][FRAME_LEN];

void historyInit(void) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		historyKeys[i] = EMPTY_KEY;
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
		if (historyKeys[slot] == key) {
			history[slot][stateIdx] = state;
			return -2;
		}
		if (historyKeys[slot] == EMPTY_KEY) {
			historyKeys[slot] = key;
			history[slot][stateIdx] = state;
			return 0;
		}
	}
	return -1;
}

int hashGet(uint32_t key) {
	uint8_t idx = hash(key);
	for (int i = 0; i < PLAYER_LEN; i++) {
		uint8_t slot = (idx + i) & (PLAYER_LEN - 1);
		if (historyKeys[slot] == key)
			return slot;
		if (historyKeys[slot] == EMPTY_KEY)
			return -1;
	}
	return -1;
}
