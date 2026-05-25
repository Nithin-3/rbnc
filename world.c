#include "world.h"
#include <stdlib.h>

Player *player[PLAYER_LEN];
Entity *entityHead[PLAYER_LEN] = { 0 };
float dt;

void insertEntity(Entity e) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		if (!entityHead[i] || entityHead[i]->color == e.color) {
			Entity *en = malloc(sizeof(Entity));
			en->x = e.x;
			en->y = e.y;
			en->color = e.color;
			en->next = entityHead[i];
			entityHead[i] = en;
			return;
		}
	}
}

void freeEntity(Entity e) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		Entity **pp = &entityHead[i];
		while (*pp) {
			if ((int)(*pp)->x == (int)e.x && (int)(*pp)->y == (int)e.y) {
				Entity *tmp = *pp;
				*pp = (*pp)->next;
				free(tmp);
				return;
			}
			pp = &(*pp)->next;
		}
	}
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
