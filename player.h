#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

void initPlayer(float x, float y, uint32_t color);
void updatePlayer(float dirX, float dirY);
uint8_t toggleDraw();

#endif
