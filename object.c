#include "object.h"

struct CacheEntry {
	SDL_Texture *texture;
	int radius;
	uint32_t color;
};

static struct CacheEntry player_cache[CIRCLE_CACHE_SIZE];
static int player_cache_n;
static struct CacheEntry entity_cache[CIRCLE_CACHE_SIZE];
static int entity_cache_n;

static SDL_Texture *make_circle_texture(SDL_Renderer *renderer, int radius, uint32_t color) {
	int diameter = radius * 2;
	SDL_Surface *surface = SDL_CreateSurface(diameter, diameter, SDL_PIXELFORMAT_RGBA8888);
	if (!surface)
		return NULL;

	Uint32 *pixels = (Uint32 *)surface->pixels;
	int pitch = surface->pitch / 4;
	for (int y = 0; y < diameter; y++) {
		for (int x = 0; x < diameter; x++) {
			float dx = x - radius;
			float dy = y - radius;
			float dist2 = dx * dx + dy * dy;
			if (dist2 <= 3 * 3 && radius > 3)
				pixels[y * pitch + x] = 0xFFFFFFFF;
			else if (dist2 <= radius * radius)
				pixels[y * pitch + x] = color;
		}
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	return texture;
}

static SDL_Texture *cache_lookup(struct CacheEntry *cache, int count, uint32_t color, int radius) {
	for (int i = 0; i < count; i++) {
		if (cache[i].color == color && cache[i].radius == radius)
			return cache[i].texture;
	}
	return NULL;
}

static void cache_put(struct CacheEntry *cache, int *count, uint32_t color, int radius, SDL_Texture *texture) {
	for (int i = 0; i < *count; i++) {
		if (cache[i].color == color) {
			SDL_DestroyTexture(cache[i].texture);
			cache[i].radius = radius;
			cache[i].texture = texture;
			return;
		}
	}
	if (*count < CIRCLE_CACHE_SIZE) {
		cache[*count].color = color;
		cache[*count].radius = radius;
		cache[*count].texture = texture;
		(*count)++;
	} else {
		SDL_DestroyTexture(cache[0].texture);
		cache[0].color = color;
		cache[0].radius = radius;
		cache[0].texture = texture;
	}
}

static void circle_cached(SDL_Renderer *renderer, struct center c, int radius, uint32_t color, struct CacheEntry *cache, int *count) {
	SDL_Texture *tex = cache_lookup(cache, *count, color, radius);
	if (!tex) {
		tex = make_circle_texture(renderer, radius, color);
		if (tex)
			cache_put(cache, count, color, radius, tex);
		else
			return;
	}
	int diameter = radius * 2;
	SDL_FRect dst = { c.x - radius, c.y - radius, (float)diameter, (float)diameter };
	SDL_RenderTexture(renderer, tex, NULL, &dst);
}

void circle_player(SDL_Renderer *renderer, struct center c, int radius, uint32_t color) {
	circle_cached(renderer, c, radius, color, player_cache, &player_cache_n);
}

void circle_entity(SDL_Renderer *renderer, struct center c, int radius, uint32_t color) {
	circle_cached(renderer, c, radius, color, entity_cache, &entity_cache_n);
}

void circle_cache_destroy(void) {
	for (int i = 0; i < player_cache_n; i++)
		SDL_DestroyTexture(player_cache[i].texture);
	for (int i = 0; i < entity_cache_n; i++)
		SDL_DestroyTexture(entity_cache[i].texture);
	player_cache_n = 0;
	entity_cache_n = 0;
}
