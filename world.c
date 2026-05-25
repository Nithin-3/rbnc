#include "world.h"
#include <stdlib.h>

Entity *ent[ENTITY_LEN][ENTITY_LEN];
Player *player[PLAYER_LEN];
float dt;

void insertEntity(int x, int y, Entity e) {
	if (x < 0 || x >= ENTITY_LEN || y < 0 || y >= ENTITY_LEN)
		return;
	Entity *en = malloc(sizeof(Entity));
	en->color = e.color;
	ent[x][y] = en;
}

void freeEntity(int x, int y) {
	if (x < 0 || x >= ENTITY_LEN || y < 0 || y >= ENTITY_LEN)
		return;
	free(ent[x][y]);
	ent[x][y] = NULL;
}

void insertPlayer(Player *p) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		if (player[i] != NULL)
			continue;
		Player *pl = malloc(sizeof(Player));
		pl->x = p->x;
		pl->y = p->y;
		pl->r = p->r;
		pl->color = p->color;
		pl->index = i;
		player[i] = pl;
		return;
	}
}

void freePlayer(int index) {
	if (index < 0 || index >= PLAYER_LEN)
		return;
	if (player[index] == NULL)
		return;
	free(player[index]);
	player[index] = NULL;
}
