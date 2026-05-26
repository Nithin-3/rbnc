#include "args.h"
#include "ws.h"
#include <libwebsockets.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#define MAX_QUEUE 10

typedef struct {
	struct lws *wsi;
	char queue[MAX_QUEUE][512];
	int head;
	int tail;
} wsClient;

wsClient wsClientGlobal = { 0 };

static struct lws_context *context = NULL;
static volatile int wsRunning = 0;
static pthread_t wsThread;

static void sigintHandler(int sig) {
	if (sig == SIGINT)
		wsRunning = 0;
}

static void *wsThreadFunc(void *arg) {
	while (wsRunning) {
		if (context)
			lws_service(context, 50);
	}
	return NULL;
}

void sendMsg(const char *msg) {
	int next = (wsClientGlobal.tail + 1) % MAX_QUEUE;

	if (next == wsClientGlobal.head) {
		printf("Queue full, dropping message\n");
		return;
	}

	strncpy(wsClientGlobal.queue[wsClientGlobal.tail],
		msg,
		sizeof(wsClientGlobal.queue[0]) - 1);

	wsClientGlobal.tail = next;

	if (wsClientGlobal.wsi)
		lws_callback_on_writable(wsClientGlobal.wsi);
}

static int callbackWs(
    struct lws *wsi,
    enum lws_callback_reasons reason,
    void *user,
    void *in,
    size_t len) {
	switch (reason) {
		case LWS_CALLBACK_CLIENT_ESTABLISHED: {
			printf("Connected to server\n");
			wsClientGlobal.wsi = wsi;
			break;
		}

		case LWS_CALLBACK_CLIENT_RECEIVE: {
			printf("Received: %.*s\n", (int)len, (char *)in);
			break;
		}

		case LWS_CALLBACK_CLIENT_WRITEABLE: {
			if (wsClientGlobal.head == wsClientGlobal.tail)
				break;

			char *msg = wsClientGlobal.queue[wsClientGlobal.head];

			unsigned char buf[LWS_PRE + 512];
			unsigned char *p = &buf[LWS_PRE];

			int n = strlen(msg);
			memcpy(p, msg, n);

			lws_write(wsi, p, n, LWS_WRITE_TEXT);

			printf("Sent: %s\n", msg);

			wsClientGlobal.head =
			    (wsClientGlobal.head + 1) % MAX_QUEUE;

			if (wsClientGlobal.head != wsClientGlobal.tail)
				lws_callback_on_writable(wsi);

			break;
		}

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			printf("Connection error\n");
			break;

		case LWS_CALLBACK_CLOSED:
			printf("Connection closed\n");
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

void wsInit(void) {
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;

	context = lws_create_context(&info);

	if (!context) {
		printf("Failed to create context\n");
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
		return;
	}

	printf("Connecting to %s:%s...\n", url, port);
}

void wsServiceLoop(void) {
	signal(SIGINT, sigintHandler);
	wsRunning = 1;
	pthread_create(&wsThread, NULL, wsThreadFunc, NULL);
}

void wsStop(void) {
	wsRunning = 0;
	pthread_join(wsThread, NULL);
}
