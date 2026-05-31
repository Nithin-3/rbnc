#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

void initPlayer(float x, float y, uint32_t color,const char name[64]);
void updatePlayer(float dirX, float dirY);
uint8_t toggleDraw();
uint32_t playerColor();

#endif
