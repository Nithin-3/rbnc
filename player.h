#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

void initPlayer(float x, float y, uint32_t color, const char name[64]);
void updatePlayer(uint8_t dir);	 // bit flags: UP=1, DOWN=2, LEFT=4, RIGHT=8
uint8_t toggleDraw();
uint32_t playerColor();
void sendPeriodicPing(void);

#endif
