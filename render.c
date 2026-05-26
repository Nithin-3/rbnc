#include "camera.h"
#include "object.h"
#include "render.h"
#include "world.h"
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

	for (int i = 0; i < PLAYER_LEN; i++) {
		if (!entityHead[i])
			break;
		Entity *head = entityHead[i];
		while (head) {
			if (head->x >= cameraPos.x && head->x < cameraPos.x + cameraPos.w && head->y >= cameraPos.y && head->y < cameraPos.y + cameraPos.h) {
				SDL_FRect r = worldEntity2Screen(head->x - 5, head->y - 5, 10, 10);
				struct center c = { .x = r.x + r.w / 2, .y = r.y + r.h / 2 };
				circle_entity(renderer, c, 3, head->color);
			}
			head = head->next;
		}
	}
}
