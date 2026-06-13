#ifndef WS_H
#define WS_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	void *data;
	size_t len;
} wsMsg;

void wsInit(void);
void sendMsg(const void *data, size_t len);
void wsPoll(void);
void wsStop(void);
int wsWaitForInit(void);

#endif
