#include "render.h"
#include "world.h"
#include "camera.h"

void render(SDL_Window *window, SDL_Renderer *renderer) {
	for (int i = 0; i < PLAYER_LEN; i++) {
		if (player[i] == NULL)
			continue;
		Player *p = player[i];
		if (p->x >= cameraPos.x && p->x < cameraPos.x + cameraPos.w && p->y >= cameraPos.y && p->y < cameraPos.y + cameraPos.h) {
			// TODO: render player
		}
	}

	for (int x = cameraPos.x; x < cameraPos.x + cameraPos.w; x++) {
		for (int y = cameraPos.y; y < cameraPos.h; y++) {
			if (ent[x][y] != NULL) {
				// TODO: render entity
			}
		}
	}
}
