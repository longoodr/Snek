#include "assets.h"
#include "myLib.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

// Static prototypes


/*
Snake is implemented as a deque (backed by a doubly
linked list) to allow for O(1) additions/removals at
the ends (for movement) as well as painless resizing
*/
Snake snake;

// Game state that holds information on score, direction
// of snake, state of board's tiles, etc
GameState state;

Mouse mouse;
// Places an apple in a random empty cell
void placeApple()
{
	// Generate random coordinate
	int r = rand() % (TILE_X * TILE_Y);

	// Regenerate coordinate as needed until
	// designated cell is empty
	while(state.board[r] != EMPTY) {
		r = (r + rand() + 1) % (TILE_X * TILE_Y);
	}

	// Place the apple
	state.board[r] = APPLE;
	const unsigned short* sprite;

	// See if mouse spawned
	if (!(mouse.spawnNum++ % MOUSE_SPAWN))
	{
		sprite = MouseUp;
		mouse.isMouse = TRUE;
		mouse.x = r % TILE_X;
		mouse.y = r / TILE_X;
	}
	else
	{
		sprite = Apple;
		mouse.isMouse = FALSE;
	}

	// Reset moves since last food eaten
	state.numMoves = 0;
	drawSprite(r % TILE_X, r / TILE_X, sprite);
}

// Sets up starting conditions of game
void initGame()
{
	static bool first = TRUE;

	// Initialize numerical states
	state.score = 100;
	state.numCollisions = MAX_COLLISIONS;
	state.lastXDir = 0;
	state.lastYDir = 0;
	state.xDir = 1;
	state.yDir = 0;

	mouse.isMouse = FALSE;
	mouse.walkCooldown = 0;
	mouse.spawnNum = 1;

	// Reset board to be completely empty
	for (int i = 0; i < TILE_X * TILE_Y; i++)
	{
		state.board[i] = EMPTY;
	}

	// Destroy old snake if it exists
	if (!first)
	{
		SnakeSeg* thisSeg = snake.head;

		// Iterate through old segments and remove each
		while (thisSeg != snake.tail)
		{
			SnakeSeg* segToFree = thisSeg;
			thisSeg = thisSeg->prev;
			free(segToFree);
		}
		free(thisSeg);
	}

	first = FALSE;
	snake.length = 0;

	// Create starting snake
	int startX = (TILE_X - STARTING_LENGTH) / 2;
	for (int i = 0; i < STARTING_LENGTH; i++)
	{
		addSeg(startX + i, TILE_Y / 2);
		if (i == 0) 
		{

			// This is the initial tail
			drawSprite(startX + i, TILE_Y / 2, TailRight);
		}
		else if (i == STARTING_LENGTH - 1)
		{
			// This is the initial head
			drawSprite(startX + i, TILE_Y / 2, HeadLeft);
		}
		else
		{
			// This is in between, so is initially body
			drawSprite(startX + i, TILE_Y / 2, BodyHorizontal);
		}
	}

	// Place the initial apple
	placeApple();
}

// Attempts to move snake and updates game accordingly
void stepGame()
{
	int xBase = snake.head->x;
	int yBase = snake.head->y;
	state.lastXDir = state.xDir;
	state.lastYDir = state.yDir;

	if (xBase + state.xDir >= TILE_X || xBase + state.xDir < 0 ||
			yBase + state.yDir >= TILE_Y || yBase + state.yDir < 0)
	{
		// Wall hit
		state.numCollisions--;
		return;
	}
	if (state.board[OFFSET(xBase + state.xDir, yBase + state.yDir, TILE_X)] == SNAKE)
	{
		// Snake hit
		state.numCollisions--;
		return;
	}

	if (state.board[OFFSET(xBase + state.xDir, yBase + state.yDir, TILE_X)] == APPLE)
	{
		// Apple hit: grow into Apple square and generate new Apple
		addSeg(xBase + state.xDir, yBase + state.yDir);

		// Render new head
		drawSprite(xBase + state.xDir, yBase + state.yDir, getSprite(snake.head));

		// Render old head as body
		drawSprite(xBase, yBase, getSprite(snake.head->prev));

		// Increase score
		state.score += mouse.isMouse ? MOUSE_SCORE : APPLE_SCORE;

		// Generate new Apple
		placeApple();
		return;
	}


	// Moved into empty tile
	SnakeSeg* oldTail = snake.tail;
	snake.tail = snake.tail->next;

	// Erase old tail
	eraseSprite(oldTail->x, oldTail->y);
	state.board[OFFSET(oldTail->x, oldTail->y, TILE_X)] = EMPTY;
	free(oldTail);
	snake.length--;
	addSeg(xBase + state.xDir, yBase + state.yDir);

	// New segment rendered as head
	drawSprite(xBase + state.xDir, yBase + state.yDir, getSprite(snake.head));

	// Rerender previous head as body
	drawSprite(xBase, yBase, getSprite(snake.head->prev));

	// Other body segments do not change appearance
	// Rerender new tail as tail
	drawSprite(snake.tail->x, snake.tail->y, getSprite(snake.tail));

	// Apply score penalty if applicable
	if (state.numMoves++ > PENALTY_DELAY)
	{
		state.score--;
	}

	// Regain grace collisions
	if (state.numCollisions < MAX_COLLISIONS)
	{
		state.numCollisions++;
	}

	// If there is a mouse, try to move it
	if (mouse.isMouse && mouse.walkCooldown-- < 0)
	{
		moveMouse();
	}
}

// Moves the mouse
// Mouse will run away from snake if snake is close,
// or will randomly walk if not close
static void moveMouse()
{
	int xDir = 0;
	int yDir = 0;

	// Compute distances
	int xDiff = mouse.x - snake.head->x;
	int yDiff = mouse.y - snake.head->y;
	int dist = abs(xDiff) + abs(yDiff);

	// Does mouse see snake?
	bool seen = dist <= MOUSE_VISION;
	if (seen)
	{
			// run in a direction that will put distance
			// between mouse and snake
			int* dir = &xDir;
			int* diff = &xDiff;
			if (!*diff || ((rand() & 1) && *diff))
			{
				dir = &yDir;
				diff = &yDiff;
			}
			*dir = (*diff > 0) ? 1 : -1;
		mouse.walkCooldown = MOUSE_RUN_CD;
	}
	else
	{
		// Snake not seen, walk randomly
		int* dir = &xDir;
		if (rand() & 1)
		{
			dir = &yDir;
		}
		*dir = (rand() & 1) ? 1 : -1;
		mouse.walkCooldown = MOUSE_WALK_CD;
	}

	bool walkOffScreen = (mouse.x + xDir >= TILE_X || mouse.x + xDir < 0 ||
			mouse.y + yDir >= TILE_Y || mouse.y + yDir < 0);

	// Attempt to walk in chosen direction
	if (!walkOffScreen && state.board[OFFSET(mouse.x + xDir, mouse.y + yDir, TILE_X)] == EMPTY)
	{
		// Erase old mouse
		eraseSprite(mouse.x, mouse.y);
		state.board[OFFSET(mouse.x, mouse.y, TILE_X)] = EMPTY;

		mouse.x += xDir;
		mouse.y += yDir;

		// Determine new sprite
		const unsigned short* sprite;
		switch (xDir)
		{
			case 1:
				sprite = MouseRight;
				break;
			case -1:
				sprite = MouseLeft;
				break;
			default:
				switch (yDir)
				{
					case 1:
						sprite = MouseDown;
						break;
					default:
						sprite = MouseUp;
						break;
				}
				break;
		}

		// Move and draw mouse
		drawSprite(mouse.x, mouse.y, sprite);
		state.board[OFFSET(mouse.x, mouse.y, TILE_X)] = APPLE;

		// If seen, try moving again at next step
		if (seen)
		{
			mouse.walkCooldown = 0;
		}
	}
	else
	{
		// Try moving again at next step
		mouse.walkCooldown = 0;
	}
}

// Adds a segment to the snake as the head
static void addSeg(int x, int y)
{

	// Create the new segment
	SnakeSeg* newHead = (SnakeSeg*)malloc(sizeof(SnakeSeg));

	// Assign coordinates
	newHead->x = x;
	newHead->y = y;

	// Flag this cell as snake
	state.board[OFFSET(x, y, TILE_X)] = SNAKE;

	if (snake.length > 0) 
	{

		// Make this segment the new head
		newHead->prev = snake.head;
		snake.head->next = newHead;
		snake.head = newHead;
	} 
	else 
	{

		// This is the only segment, so it is both
		// the head and the tail
		snake.head = newHead;
		snake.tail = newHead;


	}

	snake.length++;
}

// Determines the sprite of the given segment based
// on the relative positions of the adjacent segments
static const unsigned short* getSprite(SnakeSeg* seg)
{
	const unsigned short* sprite;
	if (seg != snake.tail && seg != snake.head) {

		// This segment has a prev and next, so both
		// need to be considered to determine sprite
		SnakeSeg* prevSeg = seg->prev;
		SnakeSeg* nextSeg = seg->next;
		int nextXDiff = seg->x - nextSeg->x;
		int nextYDiff = seg->y - nextSeg->y;
		int prevXDiff = seg->x - prevSeg->x;
		int prevYDiff = seg->y - prevSeg->y;

		// This switch determines the sprite based on
		// relative positions of the adjacent segments
		switch (nextXDiff)
		{
			case 1:
				if (prevXDiff == -1)
					sprite = BodyHorizontal;
				else if (prevYDiff == -1)
					sprite = TurnDownLeft;
				else
					sprite = TurnUpLeft;
				break;
			case -1:
				if (prevXDiff == 1)
					sprite = BodyHorizontal;
				else if (prevYDiff == -1)
					sprite = TurnDownRight;
				else
					sprite = TurnUpRight;
				break;

			default:
				switch (nextYDiff)
				{
					case -1:
						if (prevYDiff == 1)
							sprite = BodyVertical;
						else if (prevXDiff == -1)
							sprite = TurnDownRight;
						else
							sprite = TurnDownLeft;
						break;
					default:
						if (prevYDiff == -1)
							sprite = BodyVertical;
						else if (prevXDiff == -1)
							sprite = TurnUpRight;
						else
							sprite = TurnUpLeft;
						break;
				}
			break;
		}
	}
	else if (seg == snake.head) {

		// This segment is the head and only needs to
		// consider the previous segment
		SnakeSeg* prevSeg = seg->prev;

		int xDiff = seg->x - prevSeg->x;
		int yDiff = seg->y - prevSeg->y;

		switch (xDiff)
		{
			case 1:
				sprite = HeadLeft;
				break;
			case -1:
				sprite = HeadRight;
				break;
			default:
				switch (yDiff) 
				{
					case -1:
						sprite = HeadDown;
						break;
					default:
						sprite = HeadUp;
						break;
				}
				break;
		}
	}
	else
	{
		// This segment is the tail and only needs to
		// consider the next segment
		SnakeSeg* nextSeg = seg->next;

		int xDiff = seg->x - nextSeg->x;
		int yDiff = seg->y - nextSeg->y;

		switch (xDiff)
		{
			case 1:
				sprite = TailLeft;
				break;
			case -1:
				sprite = TailRight;
				break;
			default:
				switch (yDiff) 
				{
					case -1:
						sprite = TailDown;
						break;
					default:
						sprite = TailUp;
						break;
				}
				break;
		}
	}
	return sprite;
}