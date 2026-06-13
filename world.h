#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#define ENTITY_LEN 1000
#define PLAYER_LEN 8 // must be power of 2 for bitwise AND hash

typedef struct Entity {
	float x, y;
	struct Entity *next;
	uint32_t color;
} Entity;

typedef struct {
	char name[64];
	float x, y;
	uint8_t index, r;
	uint32_t color;
} Player;

extern Entity *entityHead[PLAYER_LEN], *entityTail[PLAYER_LEN];
extern Player *player[PLAYER_LEN];
extern double dt;
extern const double dtFix;
extern uint64_t sendTime, ping;
extern int awaitingPing;
extern uint32_t gameTime;

void insertEntity(Entity e);
void freeEntity(Entity e);
void freeEntityByColor(uint32_t color);
int insertPlayer(Player *p);
void freePlayer(int index);

#endif
