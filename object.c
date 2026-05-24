#include "object.h"
#include <math.h>

void circle(SDL_Renderer *renderer, struct center c, int radius, int innerfill, int thickness, uint32_t color) {
	SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
	for (int x = c.x - radius; x <= c.x + radius; x++) {
		for (int y = c.y - radius; y <= c.y + radius; y++) {
			float dx = x - c.x;
			float dy = y - c.y;
			float d = sqrtf(dx * dx + dy * dy);
			if (d <= radius) {
				if (innerfill || d >= radius - thickness)
					SDL_RenderPoint(renderer, (float)x, (float)y);
			}
		}
	}
}
