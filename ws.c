#include "args.h"
#include "camera.h"
#include "player.h"
#include "world.h"
#include "ws.h"
#include <SDL3/SDL_mutex.h>
#include <SDL3/SDL_thread.h>
#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
	struct lws *wsi;
} wsClient;

wsClient wsClientGlobal = { 0 };

static struct lws_context *context = NULL;
static volatile int wsRunning = 0;
static SDL_Thread *wsThread;
static uint64_t tim;
static SDL_Mutex *initMutex = NULL;
static SDL_Condition *initCond = NULL;
static int initDone = 0;
static int initFailed = 0;

static void sigintHandler(int sig) {
	if (sig == SIGINT)
		wsRunning = 0;
}

static int wsThreadFunc(void *arg) {
	while (wsRunning) {
		if (context)
			lws_service(context, 50);
	}
	return 0;
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

	if (wsClientGlobal.wsi) {
		lws_callback_on_writable(wsClientGlobal.wsi);
		lws_cancel_service(context);
	}
}

static int callbackWs(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch (reason) {
		case LWS_CALLBACK_CLIENT_ESTABLISHED: {
			printf("Connected to server\n");
			wsClientGlobal.wsi = wsi;
			size_t len = strlen(name);
			tim = (uint64_t)time(NULL);
			unsigned char buff[1 + len + sizeof(tim)];
			buff[0] = 0x03;
			memcpy(buff + 1, &tim, sizeof(tim));
			memcpy(buff + 1 + sizeof(tim), name, len);
			sendMsg(buff, 1 + sizeof(tim) + len);
			break;
		}

		case LWS_CALLBACK_CLIENT_RECEIVE: {
			if (len < 1)
				break;
			uint8_t type = *(uint8_t *)in;
			switch (type) {
				case 1: {
					if (len < sizeof(playerEvent))
						break;
					playerEvent *evt = (playerEvent *)in;
					Entity e = { .color = evt->color, .x = evt->x, .y = evt->y };
					insertEntity(e);
					// NOTE: it need to go case 0 don't need break;
				}
				case 0: {
					if (len < sizeof(playerEvent))
						break;
					playerEvent *evt = (playerEvent *)in;
					for (int i = 0; i < PLAYER_LEN; i++) {
						if (player[i] && evt->color == player[i]->color) {
							player[i]->x = evt->x, player[i]->y = evt->y;
							break;
						}
					}
					if (evt->color == playerColor()) {
						ping = SDL_GetTicks() - sendTime;
						awaitingPing = 0;
						updateCamera(evt->x, evt->y);
					}
					break;
				}
				case 3: {
					if (len < WS_NAME_OFF + 1)
						break;
					uint32_t color = *(uint32_t *)(in + WS_COLOR_OFF);
					float x = *(float *)(in + WS_X_OFF), y = *(float *)(in + WS_Y_OFF);
					char *incoming_name = (char *)(in + WS_NAME_OFF);
					uint64_t rtim = *(uint64_t *)(in + WS_TIME_OFF);
					incoming_name[len - WS_NAME_OFF] = '\0';
					Player p = {
						.x = x,
						.y = y,
						.r = 100,
						.color = color,
					};
					strncpy(p.name, incoming_name, sizeof(p.name) - 1);
					insertPlayer(&p);
					if (tim == rtim) {
						initPlayer(x, y, color, incoming_name);
						SDL_LockMutex(initMutex);
						initDone = 1;
						SDL_SignalCondition(initCond);
						SDL_UnlockMutex(initMutex);
					}
					printf("%u\t%f\t%f\t%s\n", color, x, y, incoming_name);
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
			break;
		}

		case LWS_CALLBACK_CLIENT_WRITEABLE: {
			if (wsClientGlobal.head == wsClientGlobal.tail)
				break;

			wsMsg *msg = &wsClientGlobal.queue[wsClientGlobal.head];

			unsigned char buf[LWS_PRE + msg->len];
			unsigned char *p = &buf[LWS_PRE];

			memcpy(p, msg->data, msg->len);

			lws_write(wsi, p, msg->len, LWS_WRITE_BINARY);

			free(msg->data);
			msg->data = NULL;

			wsClientGlobal.head = (wsClientGlobal.head + 1) % MAX_QUEUE;

			if (wsClientGlobal.head != wsClientGlobal.tail)
				lws_callback_on_writable(wsi);

			break;
		}

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			printf("Connection error\n");
			SDL_LockMutex(initMutex);
			initFailed = 1;
			SDL_SignalCondition(initCond);
			SDL_UnlockMutex(initMutex);
			break;

		case LWS_CALLBACK_CLOSED:
			printf("Connection closed\n");
			wsClientGlobal.wsi = NULL;
			SDL_LockMutex(initMutex);
			if (!initDone)
				initFailed = 1;
			SDL_SignalCondition(initCond);
			SDL_UnlockMutex(initMutex);
			break;

		default:
			break;
	}

	return 0;
}

static struct lws_protocols protocols[] = {
	{
	    "ws",
	    callbackWs,
	    0,
	    512,
	},
	{ NULL, NULL, 0, 0 }
};

int wsWaitForInit(void) {
	SDL_LockMutex(initMutex);
	while (!initDone && !initFailed)
		SDL_WaitCondition(initCond, initMutex);
	SDL_UnlockMutex(initMutex);
	return initDone;
}

void wsInit(void) {
	initMutex = SDL_CreateMutex();
	initCond = SDL_CreateCondition();
	initDone = 0;
	initFailed = 0;

	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;

	context = lws_create_context(&info);

	if (!context) {
		printf("Failed to create context\n");
		SDL_LockMutex(initMutex);
		initFailed = 1;
		SDL_SignalCondition(initCond);
		SDL_UnlockMutex(initMutex);
		return;
	}

	struct lws_client_connect_info ccinfo = { 0 };

	ccinfo.context = context;
	ccinfo.address = url;
	ccinfo.port = atoi(port);
	ccinfo.path = "/";
	ccinfo.host = ccinfo.address;
	ccinfo.origin = ccinfo.address;
	ccinfo.protocol = protocols[0].name;

	if (!lws_client_connect_via_info(&ccinfo)) {
		printf("Connection failed\n");
		SDL_LockMutex(initMutex);
		initFailed = 1;
		SDL_SignalCondition(initCond);
		SDL_UnlockMutex(initMutex);
		return;
	}

	printf("Connecting to %s:%s...\n", url, port);
}

void wsServiceLoop(void) {
	signal(SIGINT, sigintHandler);
	wsRunning = 1;
	wsThread = SDL_CreateThread(wsThreadFunc, "ws", NULL);
}

void wsStop(void) {
	if (context)
		lws_cancel_service(context);
	wsRunning = 0;
	SDL_WaitThread(wsThread, NULL);
	if (context) {
		lws_context_destroy(context);
		context = NULL;
	}
	if (initMutex) {
		SDL_DestroyMutex(initMutex);
		initMutex = NULL;
	}
	if (initCond) {
		SDL_DestroyCondition(initCond);
		initCond = NULL;
	}
}
