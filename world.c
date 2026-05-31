#include "world.h"
#include <stdlib.h>
#include <string.h>

Player *player[PLAYER_LEN];
Entity *entityHead[PLAYER_LEN] = { 0 }, *entityTail[PLAYER_LEN] = { 0 };
float dt;

void insertEntity(Entity e) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		if (!entityHead[i] || entityHead[i]->color == e.color) {
			Entity *en = malloc(sizeof(Entity));
			en->x = e.x;
			en->y = e.y;
			en->color = e.color;

			Entity *prev = NULL, *cur = entityHead[i];
			while (cur) {
				if ((int)cur->x == (int)e.x && (int)cur->y == (int)e.y) {
					free(en);
					return;
				}
				if (cur->x > e.x || (cur->x == e.x && cur->y > e.y))
					break;
				prev = cur;
				cur = cur->next;
			}
			en->next = cur;
			if (prev)
				prev->next = en;
			else
				entityHead[i] = en;
			if (!cur)
				entityTail[i] = en;
			return;
		}
	}
}

void freeEntity(Entity e) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		Entity *prev = NULL, *cur = entityHead[i];
		while (cur) {
			if ((int)cur->x == (int)e.x && (int)cur->y == (int)e.y) {
				if (prev)
					prev->next = cur->next;
				else
					entityHead[i] = cur->next;
				if (cur == entityTail[i])
					entityTail[i] = prev;
				free(cur);
				return;
			}
			prev = cur;
			cur = cur->next;
		}
	}
}

void freeEntityByColor(uint32_t color) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		Entity *cur = entityHead[i];
		while (cur && cur->color == color) {
			Entity *next = cur->next;
			free(cur);
			cur = next;
		}
		if (entityHead[i] && entityHead[i]->color == color) {
			entityHead[i] = cur;
			if (!cur)
				entityTail[i] = NULL;
		}
	}
}

int insertPlayer(Player *p) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		if (NULL == player[i] || p->color == player[i]->color) {
			Player *pl = malloc(sizeof(Player));
			pl->x = p->x;
			pl->y = p->y;
			pl->r = p->r;
			pl->color = p->color;
			pl->index = i;
			strncpy(pl->name, p->name, sizeof(pl->name) - 1);
			if (player[i])
				free(player[i]);
			player[i] = pl;
			return i;
		}
	}
	return -1;
}

void freePlayer(int index) {
	if (index < 0 || index >= PLAYER_LEN)
		return;
	if (player[index] == NULL)
		return;
	free(player[index]);
	player[index] = NULL;
}
