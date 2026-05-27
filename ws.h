#ifndef WS_H
#define WS_H

#include <stdint.h>

typedef struct {
	uint8_t type;
	float x, y;
	uint32_t color;
} __attribute__((packed)) playerEvent;

void wsInit(void);
void sendMsg(const playerEvent *event);
void wsServiceLoop(void);
void wsStop(void);

#endif
