#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assets.h"
#include "myLib.h"
#include "main.h"



// Action that the game loop repeatedly calls
void (*currentAction)(void);

static int counter = 0;		//counter used for timing

int main()
{
	// Initialize state of game
	initScreen();
	enterSplash();

	// Repeatedly call whatever action is associated
	// with the current state via function pointer
	while(1)
	{
		(*currentAction)();
	}
}

// Changes game state to initial main menu
void enterSplash()
{
	currentAction = &actionSplash;
	waitForVblank();
	drawImage3(0, 0, 240, 160, TitleScreen);
}

// Changes game state to the game itself
void enterGame()
{
	counter = 180;
	currentAction = &actionCountdown;
	waitForVblank();
	drawRect(0, 0, 240, 160, WHITE);
	initGame();
}

// Performs intro countdown to game start
void actionCountdown()
{
	// Go to main menu when select is pressed
	if (KEY_DOWN_NOW(BUTTON_SELECT))
	{
		enterSplash();
		return;
	}
	waitForVblank();

	// Every 60 frames (1 second) update countdown
	if (counter-- % 60 == 0)
	{
		char buffer[1];
		sprintf(buffer, "%d", 1 + counter / 60);
		drawRect(0, 140, 240, 20, BLACK);
		drawString(SCRN_WIDTH / 2 - 3, 145, buffer, WHITE);

		// Enter game once countdown complete
		if (counter <= 0)
		{
			currentAction = &actionGame;
		}
	}
}

// Changes game state to the game over screen
void enterOver()
{
	counter = 0;
	currentAction = &actionOver;
	waitForVblank();

	// Show game over image
	drawImage3(0, 0, 240, 160, GameOver);
}

// Continually runs in game over screen
void actionOver()
{
	// Go to main menu when select is pressed
	if (KEY_DOWN_NOW(BUTTON_SELECT))
	{
		enterSplash();
		return;
	}
	waitForVblank();

	// Write final score after 1 second
	if (counter++ == 60)
	{
		char buffer[40];
		sprintf(buffer, "Final Score: %-6d Final Length: %-3d", state.score, snake.length);
		drawRect(0, 140, 240, 20, BLACK);
		waitForVblank();
		drawString(SCRN_WIDTH / 2 - (strlen(buffer)) * 3, 145, buffer, WHITE);
	}
}

// Continually runs in main menu
void actionSplash()
{
	// Starting seed is completely arbitrary
	static int delay = 0xDEADBEEF;
	if (KEY_DOWN_NOW(BUTTON_START))
	{
		// Seed rand based on input delay after starting
		srand(delay);
		enterGame();
		return;
	}
	delay++;
}

// Continually runs in game
void actionGame()
{
	waitForVblank();
	// Check if any buttons are pressed
	if (~BUTTONS)
	{
		// Go to main menu when select is pressed
		if (KEY_DOWN_NOW(BUTTON_SELECT))
		{
			enterSplash();
			return;
		}

		// Change snake direction based on D-Pad input
		if (KEY_DOWN_NOW(BUTTON_DOWN) && state.lastYDir != -1)
		{
			state.xDir = 0;
			state.yDir = 1;
		} 
		if (KEY_DOWN_NOW(BUTTON_UP) && state.lastYDir != 1)
		{
			state.xDir = 0;
			state.yDir = -1;
		} 
		if (KEY_DOWN_NOW(BUTTON_LEFT) && state.lastXDir != 1)
		{
			state.xDir = -1;
			state.yDir = 0;
		} 
		if (KEY_DOWN_NOW(BUTTON_RIGHT) && state.lastXDir != -1)
		{
			state.xDir = 1;
			state.yDir = 0;
		}
	}

	counter = (counter + 1) % MOVE_FREQ;

	// Only move snake every few frames
	if (counter == 0)
	{
		stepGame();

		// Check if game is over
		if (state.numCollisions <= 0)
		{
			enterOver();
			return;
		}	
		displayScore();
	}
}

// Displays the current score at bottom of screen
void displayScore()
{
	char buffer[40];
	sprintf(buffer, "Score: %-6d   Length: %-3d", state.score, snake.length);
	drawRect(0, 140, 240, 20, BLACK);
	drawString(SCRN_WIDTH / 2 - 81, 145, buffer, WHITE);
}