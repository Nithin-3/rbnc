#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#define ENTITY_LEN 1000
#define PLAYER_LEN 10

typedef struct Entity {
	float x, y;
	struct Entity *next;
	uint32_t color;
} Entity;

typedef struct {
	float x, y;
	int index, r;
	uint32_t color;
} Player;

extern Player *player[PLAYER_LEN];
extern Entity *entityHead[PLAYER_LEN], *entityTail[PLAYER_LEN];
extern float dt;

void insertEntity(Entity e);
void freeEntity(Entity e);
void insertPlayer(Player *p);
void freePlayer(int index);

#endif
