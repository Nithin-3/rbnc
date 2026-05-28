#ifndef WS_H
#define WS_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t type;
	float x, y;
	uint32_t color;
} __attribute__((packed)) playerEvent;

typedef struct {
	void *data;
	size_t len;
} wsMsg;

void wsInit(void);
void sendMsg(const void *data, size_t len);
void wsServiceLoop(void);
void wsStop(void);

#endif
