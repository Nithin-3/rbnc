#ifndef WS_H
#define WS_H

#include <stdint.h>
#include <stddef.h>

#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif
typedef struct {
	uint8_t type;
	float x, y;
	uint32_t color;
} 
#if !defined(_MSC_VER)
__attribute__((packed))
#endif
playerEvent;
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

typedef struct {
	void *data;
	size_t len;
} wsMsg;

void wsInit(void);
void sendMsg(const void *data, size_t len);
void wsServiceLoop(void);
void wsStop(void);

#endif
