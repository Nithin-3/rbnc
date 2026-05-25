#include "render.h"
#include "object.h"
#include "world.h"
#include "camera.h"
#include <stdlib.h>

void render(SDL_Window *window, SDL_Renderer *renderer) {
	if (cameraPos.w == '\0' || cameraPos.h == '\0') {
		SDL_Log("camera not initialized");
		exit(1);
	}
	for (int i = 0; i < PLAYER_LEN; i++) {
		if (player[i] == NULL)
			continue;
		Player *p = player[i];
		if (p->x >= cameraPos.x && p->x < cameraPos.x + cameraPos.w && p->y >= cameraPos.y && p->y < cameraPos.y + cameraPos.h) {
			SDL_FRect r = worldEntity2Screen((int)(p->x - p->r), (int)(p->y - p->r), p->r * 2, p->r * 2);
			struct center c = { .x = r.x + r.w / 2, .y = r.y + r.h / 2 };
			circle_player(renderer, c, p->r, p->color);
		}
	}

	int sx = cameraPos.x < 0 ? 0 : (int)cameraPos.x;
	int sy = cameraPos.y < 0 ? 0 : (int)cameraPos.y;
	int ex = cameraPos.x + cameraPos.w > ENTITY_LEN
		? ENTITY_LEN : (int)(cameraPos.x + cameraPos.w);
	int ey = cameraPos.y + cameraPos.h > ENTITY_LEN
		? ENTITY_LEN : (int)(cameraPos.y + cameraPos.h);
	for (int x = sx; x < ex; x++) {
		for (int y = sy; y < ey; y++) {
			if (ent[x][y] != NULL) {
				Entity *e = ent[x][y];
				SDL_FRect r = worldEntity2Screen(x - 5, y - 5, 10, 10);
				struct center c = { .x = r.x + r.w / 2, .y = r.y + r.h / 2 };
				circle_entity(renderer, c, 33, e->color);
			}
		}
	}
}
