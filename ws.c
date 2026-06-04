#include "args.h"
#include "camera.h"
#include "input.h"
#include "player.h"
#include "world.h"
#include "ws.h"
#include <curl/curl.h>
#include <math.h>
#include <poll.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define speed 50
#define MAX_QUEUE 500

#define WS_TYPE_SIZE sizeof(uint8_t)
#define WS_COLOR_OFF (WS_TYPE_SIZE)
#define WS_X_OFF (WS_COLOR_OFF + sizeof(uint32_t))
#define WS_Y_OFF (WS_X_OFF + sizeof(float))
#define WS_TIME_OFF (WS_Y_OFF + sizeof(float))
#define WS_NAME_OFF (WS_TIME_OFF + sizeof(uint64_t))

typedef struct {
	int head;
	int tail;
	wsMsg queue[MAX_QUEUE];
} wsClient;

static wsClient wsClientGlobal = { 0 };
static CURL *curl = NULL;
static curl_socket_t sockfd = CURL_SOCKET_BAD;
static int connected = 0;
static int initDone = 0;
static uint64_t tim;

static void buildWsUrl(char *buf, size_t size) {
	const char *p = port ? port : "80";

	if (url && strstr(url, "://")) {
		if (strncmp(url, "http://", 7) == 0)
			snprintf(buf, size, "ws://%s", url + 7);
		else if (strncmp(url, "https://", 8) == 0)
			snprintf(buf, size, "wss://%s", url + 8);
		else
			snprintf(buf, size, "%s", url);
	} else if (url) {
		snprintf(buf, size, "ws://%s:%s/", url, p);
	} else {
		snprintf(buf, size, "ws://%s:%s/", ip, p);
	}
}

void sendMsg(const void *data, size_t len) {
	int next = (wsClientGlobal.tail + 1) % MAX_QUEUE;
	if (next == wsClientGlobal.head) {
		printf("Queue full, dropping message\n");
		return;
	}
	wsClientGlobal.queue[wsClientGlobal.tail].data = malloc(len);
	memcpy(wsClientGlobal.queue[wsClientGlobal.tail].data, data, len);
	wsClientGlobal.queue[wsClientGlobal.tail].len = len;
	wsClientGlobal.tail = next;
}

static void handleReceive(const unsigned char *in, size_t len) {
	if (len < 1)
		return;
	uint8_t type = in[0];
	switch (type) {
		case 1:
		case 0: {
			if (len < sizeof(playerEvent))
				break;
			playerEvent *evt = (playerEvent *)in;
			if (evt->dir == 0)
				break;
			uint8_t i;
			for (i = 0; (i < PLAYER_LEN - 1 && player[i] && evt->color != player[i]->color); i++)
				;
			if (!player[i])	 // check player availablity
				break;

			// FIX:
			// server have to calculate player position and draw state
			// response type color x y seq
			// server and game have to share same fixed delta 
			// update player x y draw

			float dirX = 0, dirY = 0;

			if (evt->dir & DIR_UP) dirY -= 1;
			if (evt->dir & DIR_DOWN) dirY += 1;
			if (evt->dir & DIR_LEFT) dirX -= 1;
			if (evt->dir & DIR_RIGHT) dirX += 1;

			float len = sqrtf(dirX * dirX + dirY * dirY);
			if (len <= 0)
				return;
			float x = player[i]->x + (dirX / len) * speed * dtFix,
			      y = player[i]->y + (dirY / len) * speed * dtFix;
			// TODO: update player x,y
			// save to rbnc stack
			// check sequence orderby sequence
			//
			player[i]->x = x, player[i]->y = y;
			if (type) {
				Entity e = { .color = evt->color, .x = player[i]->x, .y = player[i]->y };
				insertEntity(e);
			}
			if (evt->color == playerColor())
				updateCamera(x, y);
			break;
		}
		case 2: {
			uint32_t color = *(uint32_t *)(in + WS_COLOR_OFF);
			if (color == playerColor()) {
				ping = SDL_GetTicks() - sendTime;
				awaitingPing = 0;
			}
			break;
		}
		case 3: {
			if (len < WS_NAME_OFF + 1)
				break;
			uint32_t color = *(uint32_t *)(in + WS_COLOR_OFF);
			float x = *(float *)(in + WS_X_OFF), y = *(float *)(in + WS_Y_OFF);
			char namebuf[64];
			uint64_t rtim = *(uint64_t *)(in + WS_TIME_OFF);
			size_t name_len = len - WS_NAME_OFF;
			if (name_len >= sizeof(namebuf))
				name_len = sizeof(namebuf) - 1;
			memcpy(namebuf, in + WS_NAME_OFF, name_len);
			namebuf[name_len] = '\0';
			Player p = { .x = x, .y = y, .r = 100, .color = color };
			strncpy(p.name, namebuf, sizeof(p.name) - 1);
			insertPlayer(&p);
			if (tim == rtim) {
				initPlayer(x, y, color, namebuf);
				initDone = 1;
			}
			printf("%u\t%f\t%f\t%s\n", color, x, y, namebuf);
			break;
		}
		case 4: {
			if (len < 5)
				break;
			uint32_t leaveColor = *(uint32_t *)(in + 1);
			for (int i = 0; i < PLAYER_LEN; i++) {
				if (player[i] && player[i]->color == leaveColor) {
					freePlayer(i);
					break;
				}
			}
			freeEntityByColor(leaveColor);
			break;
		}
	}
}

static void sendQueuedMessages(void) {
	while (wsClientGlobal.head != wsClientGlobal.tail) {
		wsMsg *msg = &wsClientGlobal.queue[wsClientGlobal.head];
		size_t sent = 0;
		CURLcode res = curl_ws_send(curl, msg->data, msg->len, &sent, 0, CURLWS_BINARY);
		if (res == CURLE_AGAIN)
			return;
		if (res != CURLE_OK) {
			printf("Send error: %s\n", curl_easy_strerror(res));
			return;
		}
		if (sent < msg->len) {
			memmove(msg->data, (char *)msg->data + sent, msg->len - sent);
			msg->len -= sent;
			return;
		}
		free(msg->data);
		msg->data = NULL;
		wsClientGlobal.head = (wsClientGlobal.head + 1) % MAX_QUEUE;
	}
}

void wsPoll(void) {
	if (!connected || sockfd == CURL_SOCKET_BAD)
		return;
	struct pollfd pfd = { .fd = sockfd, .events = POLLIN, .revents = 0 };
	int ret = poll(&pfd, 1, 0);
	if (ret > 0 && (pfd.revents & (POLLIN | POLLHUP | POLLERR))) {
		unsigned char buf[4096];
		size_t n;
		const struct curl_ws_frame *meta;
		CURLcode res = curl_ws_recv(curl, buf, sizeof(buf), &n, &meta);
		if (res == CURLE_OK) {
			if (meta->flags & CURLWS_CLOSE) {
				printf("Server closed connection\n");
				connected = 0;
			} else {
				handleReceive(buf, n);
			}
		} else if (res == CURLE_GOT_NOTHING) {
			printf("Connection closed\n");
			connected = 0;
		} else if (res != CURLE_AGAIN) {
			printf("Receive error: %s\n", curl_easy_strerror(res));
		}
	}
	sendQueuedMessages();
}

int wsWaitForInit(void) {
	uint64_t start = SDL_GetTicks();
	while (!initDone && connected) {
		if (SDL_GetTicks() - start > 5000) {
			printf("Init timeout\n");
			return 0;
		}
		wsPoll();
		SDL_Delay(1);
	}
	return initDone;
}

void wsInit(void) {
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (!curl) {
		printf("Failed to create curl handle\n");
		return;
	}

	char ws_url[512];
	buildWsUrl(ws_url, sizeof(ws_url));
	curl_easy_setopt(curl, CURLOPT_URL, ws_url);
	curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);
	printf("Connecting to %s...\n", ws_url);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		printf("Connection failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		curl = NULL;
		return;
	}
	curl_easy_getinfo(curl, CURLINFO_ACTIVESOCKET, &sockfd);
	connected = 1;

	tim = (uint64_t)time(NULL);
	size_t name_len = strlen(name);
	unsigned char buff[1 + sizeof(tim) + name_len];
	buff[0] = 0x03;
	memcpy(buff + 1, &tim, sizeof(tim));
	memcpy(buff + 1 + sizeof(tim), name, name_len);
	sendMsg(buff, 1 + sizeof(tim) + name_len);
	printf("Connected to %s\n", ws_url);
}

void wsStop(void) {
	if (curl) {
		size_t sent;
		curl_ws_send(curl, "", 0, &sent, 0, CURLWS_CLOSE);
		curl_easy_cleanup(curl);
		curl = NULL;
	}
	curl_global_cleanup();
	connected = 0;
	sockfd = CURL_SOCKET_BAD;
}
