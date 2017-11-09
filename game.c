#include "assets.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

#define STARTING_LENGTH 4

enum Tile board[TILE_X * TILE_Y] = {0};

// Node for a snake segment
struct snakeSeg {
	struct snakeSeg* next;
	struct snakeSeg* prev;
	int x;
	int y;
};

static const unsigned short* getSprite(struct snakeSeg* seg);
static void addSeg(int x, int y);

// Snake is a deque, implemented as doubly linked list
// to allow for O(1) adds/removes at head and tail as
// well as dynamic resizing
unsigned short snakeLength;
static struct snakeSeg* head;
static struct snakeSeg* tail;
static unsigned int randNum;


void seed(unsigned int seed)
{
	randNum = seed;
}

static unsigned int getRand() {

	// Seed is *completely* arbitrary

	// Perform some basic xorshifts
	// Not *super* robust but it works
	randNum ^= randNum << 3;
	randNum ^= randNum >> 5;
	randNum ^= randNum << 7;
	return randNum;
}

// Places an apple in a random empty cell
void placeApple()
{
	int r = getRand() % (TILE_X * TILE_Y);
	while(board[r] != EMPTY) {
		r = (r + getRand() + 1) % (TILE_X * TILE_Y);
	}
	board[r] = APPLE;
	drawSprite(r % TILE_X, r / TILE_X, Apple);
}

// Creates initial snake
void createSnake()
{
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
}

// Moves the snake in the given direction
// Returns the tile the snake attempted to
// move into (only successful if empty or apple)
enum Tile moveBody(int xDir, int yDir)
{
	int xBase = head->x;
	int yBase = head->y;

	if (xBase + xDir >= TILE_X || xBase + xDir < 0 ||
			yBase + yDir >= TILE_Y || yBase + yDir < 0)
	{
		// Wall hit
		return WALL;
	}
	if (board[OFFSET(xBase + xDir, yBase + yDir, TILE_X)] == SNAKE)
	{
		// Snake hit
		return SNAKE;
	}

	if (board[OFFSET(xBase + xDir, yBase + yDir, TILE_X)] == APPLE)
	{
		// Apple hit: grow into Apple square and generate new Apple
		addSeg(xBase + xDir, yBase + yDir);

		// Render new head
		drawSprite(xBase + xDir, yBase + yDir, getSprite(head));

		// Render old head as body
		drawSprite(xBase, yBase, getSprite(head->prev));

		// Generate new Apple
		placeApple();

		return APPLE;
	}

	// Moved into empty tile
	struct snakeSeg* oldTail = tail;
	tail = tail->next;

	// Erase old tail
	eraseSprite(oldTail->x, oldTail->y);
	board[OFFSET(oldTail->x, oldTail->y, TILE_X)] = EMPTY;
	free(oldTail);
	snakeLength--;
	addSeg(xBase + xDir, yBase + yDir);

	// New segment rendered as head
	drawSprite(xBase + xDir, yBase + yDir, getSprite(head));

	// Rerender previous head as body
	drawSprite(xBase, yBase, getSprite(head->prev));

	// Other body segments do not change appearance
	// Rerender new tail as tail
	drawSprite(tail->x, tail->y, getSprite(tail));
	return EMPTY;
}

// Adds a segment to the snake
static void addSeg(int x, int y)
{
	struct snakeSeg* newHead = (struct snakeSeg*)malloc(sizeof(struct snakeSeg));
	newHead->x = x;
	newHead->y = y;
	if (snakeLength > 0) 
	{
		newHead->prev = head;
		head->next = newHead;
		head = newHead;
	} 
	else 
	{
		head = newHead;
		tail = newHead;
	}
	snakeLength++;
	board[OFFSET(x, y, TILE_X)] = SNAKE;
}

// Determines the sprite of the given segment based
// on the relative positions of the adjacent segments
static const unsigned short* getSprite(struct snakeSeg* seg)
{
	const unsigned short* sprite;
	if (seg != tail && seg != head) {

		// This segment has a prev and next

		struct snakeSeg* prevSeg = seg->prev;
		struct snakeSeg* nextSeg = seg->next;
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
	else if (seg == head) {

		// This segment is the head and only needs to
		// consider the previous segment

		struct snakeSeg* prevSeg = seg->prev;

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

		struct snakeSeg* nextSeg = seg->next;

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