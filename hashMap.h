#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>

#define FRAME_LEN 10
#define EMPTY_KEY 0xFF

typedef struct {
	float x, y;
	uint8_t draw, done;
} STATE;

typedef struct {
	uint8_t key;
	STATE val[FRAME_LEN];
} HASH;

extern HASH history[];

void historyInit(void);
int hashSet(uint32_t key, STATE state, int stateIdx);
HASH *hashGet(uint32_t key);

#endif
