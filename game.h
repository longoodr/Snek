#ifndef GAME_H
#define GAME_H
#include "screen.h"

enum Tile
{
	EMPTY,
	APPLE,
	SNAKE,
	WALL
};



extern enum Tile board[TILE_X * TILE_Y];
unsigned short snakeLength;

void placeApple();
void createSnake();
void seed(unsigned int seed);
enum Tile moveBody(int xDir, int yDir);
#endif