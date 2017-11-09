#include "font.h"
#include "screen.h"
#include "inputs.h"
#include "game.h"
#include <stdio.h>
#include "assets.h"

#define APPLE_SCORE 100
#define MAX_COLLISIONS 3

void enterSplash();
void enterGame();
void enterOver();
void actionSplash();
void actionGame();
void actionOver();
void displayScore();

void (*currentAction)(void);

int score = 100;
int numCollisions = MAX_COLLISIONS;
int counter = 0;
int lastXDir = 0;
int lastYDir = 0;
int xDir = 1;
int yDir = 0;

int main()
{
	initScreen();
	enterSplash();
	while(1)
	{
		(*currentAction)();
	}
}

void enterSplash()
{
	currentAction = &actionSplash;
	waitForVblank();
	drawImage3(0, 0, 240, 160, TitleScreen);
}

void enterGame()
{
	score = 100;
	numCollisions = MAX_COLLISIONS;
	counter = 0;
	lastXDir = 0;
	lastYDir = 0;
	xDir = 1;
	yDir = 0;

	currentAction = &actionGame;
	waitForVblank();
	drawRect(0, 0, 240, 160, WHITE);
	createSnake();
	placeApple();
	displayScore();
}

void enterOver()
{
	currentAction = &actionOver;
	waitForVblank();
	drawImage3(0, 0, 240, 160, GameOver);
}

void actionOver()
{
	if (KEY_DOWN_NOW(BUTTON_SELECT))
	{
		enterSplash();
	}
}

void actionSplash()
{
	// Starting delay value is *completely* arbitrary
	static volatile int delay = 0xB16B00B5;
	if (KEY_DOWN_NOW(BUTTON_START))
	{
		seed(delay);
		enterGame();
	}
	delay++;
}

void actionGame()
{
	if (KEY_DOWN_NOW(BUTTON_SELECT))
	{
		enterSplash();
		return;
	}
	if (KEY_DOWN_NOW(BUTTON_DOWN) && lastYDir != -1)
	{
		xDir = 0;
		yDir = 1;
	} 
	if (KEY_DOWN_NOW(BUTTON_UP) && lastYDir != 1)
	{
		xDir = 0;
		yDir = -1;
	} 
	if (KEY_DOWN_NOW(BUTTON_LEFT) && lastXDir != 1)
	{
		xDir = -1;
		yDir = 0;
	} 
	if (KEY_DOWN_NOW(BUTTON_RIGHT) && lastXDir != -1)
	{
		xDir = 1;
		yDir = 0;
	}
	waitForVblank();
	if (counter++ % 10 == 0)
	{
		enum Tile result = moveBody(xDir, yDir);
		switch (result) 
		{
			case APPLE:
				score += APPLE_SCORE;
				break;
			case WALL:
			case SNAKE:
				if (numCollisions-- <= 0)
				{
					enterOver();
					return;
				}

			default:
				score--;
				if (numCollisions < MAX_COLLISIONS)
				{
					numCollisions++;
				}
		}
		displayScore();
		lastXDir = xDir;
		lastYDir = yDir;
	}
}

void displayScore()
{
	char buffer[40];
	sprintf(buffer, "Score: %-6d              Length: %-4d", score, snakeLength);
	drawRect(0, 140, 240, 20, WHITE);
	drawString(0, 145, buffer, BLACK);
}