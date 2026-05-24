#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#define ENTITY_LEN 1000
#define PLAYER_LEN 10

typedef struct {
	uint32_t color;
} Entity;

typedef struct {
	float x, y;
	int index, r;
	uint32_t color;
} Player;

extern Entity *ent[ENTITY_LEN][ENTITY_LEN];
extern Player *player[PLAYER_LEN];
extern float dt;

void insertEntity(int x, int y, Entity e);
void freeEntity(int x, int y);
void insertPlayer(Player p);
void freePlayer(int index);

#endif
