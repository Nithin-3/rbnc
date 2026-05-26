#ifndef WS_H
#define WS_H

void wsInit(void);
void sendMsg(const char *msg);
void wsServiceLoop(void);
void wsStop(void);

#endif
