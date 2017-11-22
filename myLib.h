#ifndef MYLIB_H
#define MYLIB_H

#define TILE_X 24			// number of tile cols
#define TILE_Y 14			// number of tile rows

// Different types of tiles for board cells
typedef enum
{
	EMPTY,
	APPLE,
	SNAKE,
	WALL
} Tile;

// Node for snake - implemented as a doubly linked list
typedef struct SnakeSeg
{
	struct SnakeSeg* next;
	struct SnakeSeg* prev;
	int x;
	int y;
} SnakeSeg;

// Holds information on current state of game
typedef struct 
{
	Tile board[TILE_X * TILE_Y];	// information of contents of each cell
	int score;						// current score
	int numCollisions;				// number of grace collisions remaining
	int lastXDir;					// x direction taken in previous move
	int lastYDir;					// y direction taken in previous move
	int xDir;						// x direction to take
	int yDir;						// y direction to take
	int numMoves;					// number of moves since last food eaten
} GameState;

// Holds information for mouse when food is mouse
typedef struct
{
	int x;
	int y;
	int isMouse;
	int walkCooldown;
	int spawnNum;
} Mouse;

// Holds information on the snake linked list
typedef struct 
{
	int length;
	SnakeSeg* head;
	SnakeSeg* tail;
} Snake;

// DMA abstraction
typedef struct
{
	const volatile void *src;
	volatile void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

typedef unsigned int u32;
typedef int bool;

extern unsigned short* videoBuffer;
extern GameState state;
extern Snake snake;

void initGame();
void stepGame();

void waitForVblank();
void drawRect(int x, int y, int height, int width, unsigned short color);
void drawSprite(int tileX, int tileY, const unsigned short* sprite);
void drawImage3(int x, int y, int width, int height, const unsigned short* image);
void drawChar(int x, int y, char ch, unsigned short color);
void drawString(int x, int y, char* string, unsigned short color);
void eraseSprite(int tileX, int tileY);
void initScreen();

#define TRUE 1
#define FALSE 0

#define TILE_SIZE 10		// size of each tile in pixels
#define MOVE_FREQ 4			// number of frames between moves
#define APPLE_SCORE 100		// score each apple awards
#define MOUSE_SCORE 300		// score each mouse awards
#define MAX_COLLISIONS 3	// max number of grace collisions
#define MOUSE_SPAWN 5		// controls mouse spawn frequency (lower = more)
#define MOUSE_WALK_CD 5		// controls mouse walk speed (lower = faster)
#define MOUSE_RUN_CD 1		// controls mouse run speed (lower = faster)
#define MOUSE_VISION 9		// mouse runs from snake within this distance
#define PENALTY_DELAY 40	// number of moves before score penalty starts
#define STARTING_LENGTH 4	// length of snake at start of game

#define REG_DISPCTL *(unsigned short *)0x4000000
#define MODE3 3
#define SCANLINECOUNTER *(volatile unsigned short *) 0x4000006
#define BG2_ENABLE (1<<10)
#define COLOR(r, g, b) ((r) | (g)<<5 | (b)<<10)

#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define WHITE COLOR(31, 31, 31)
#define YELLOW COLOR(31, 31,0)
#define OFFSET(x, y, xLength) ((y) * (xLength) + x)
#define BLACK 0

#define SCRN_HEIGHT 160
#define SCRN_WIDTH 240

#define INPUTS_H
#define BUTTON_A		(1<<0)
#define BUTTON_B		(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP		(1<<6)
#define BUTTON_DOWN		(1<<7)
#define BUTTON_R		(1<<8)
#define BUTTON_L		(1<<9)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

#define BUTTONS *(volatile unsigned int *)0x4000130

/*
-----------------------------------------------------
					DMA Definitions
-----------------------------------------------------
*/

// DMA channel 0 register definitions
#define REG_DMA0SAD         *(volatile u32*)0x40000B0 		// source address
#define REG_DMA0DAD         *(volatile u32*)0x40000B4       // destination address
#define REG_DMA0CNT         *(volatile u32*)0x40000B8       // control register

// DMA channel 1 register definitions
#define REG_DMA1SAD         *(volatile u32*)0x40000BC 		// source address
#define REG_DMA1DAD         *(volatile u32*)0x40000C0       // destination address
#define REG_DMA1CNT         *(volatile u32*)0x40000C4       // control register

// DMA channel 2 register definitions
#define REG_DMA2SAD         *(volatile u32*)0x40000C8 		// source address
#define REG_DMA2DAD         *(volatile u32*)0x40000CC       // destination address
#define REG_DMA2CNT         *(volatile u32*)0x40000D0       // control register

// DMA channel 3 register definitions
#define REG_DMA3SAD         *(volatile u32*)0x40000D4 		// source address
#define REG_DMA3DAD         *(volatile u32*)0x40000D8       // destination address
#define REG_DMA3CNT         *(volatile u32*)0x40000DC       // control register

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)


#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)
#endif