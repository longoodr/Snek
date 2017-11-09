#include "screen.h"
#include "font.h"

typedef unsigned int u32;

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


typedef struct
{
	const volatile void *src;
	volatile void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines
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

unsigned short* videoBuffer = (unsigned short *)0x6000000;
void drawImage3(int x, int y, int width, int height, const unsigned short* image);

void drawRect(int x, int y, int width, int height, unsigned short color)
{
	DMA[3].src = &color;
	for (int i = 0; i < height; i++)
	{
		DMA[3].dst = videoBuffer + OFFSET(x, y + i, SCRN_WIDTH);
		DMA[3].cnt = width | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED | DMA_ON;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

void drawChar(int x, int y, char ch, unsigned short color)
{

	for(int xx = 0; xx < 6; xx++)
	{
		for(int yy = 0; yy < 8; yy++)
		{
			if (fontdata_6x8[OFFSET(xx, yy, 6) + ch*48] == 1)
			{
				SET_PIXEL(x + xx, y + yy, color);
			}
		}

	}
}

void drawString(int x, int y, char* string, unsigned short color)
{
	int i = 0;
	while(*string)
	{
		drawChar(x + 6 * i++, y, *string++, color);
	}
}

// Draws a component of the snake (with a fixed size equal to size of a tile)
void drawSprite(int tileX, int tileY, const unsigned short* sprite)
{
	drawImage3(tileX * TILE_SIZE, tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE, sprite);
}

// Draws an arbitrary image using DMA
void drawImage3(int x, int y, int width, int height, const unsigned short* image)
{
	for (int i = 0; i < height; i++)
	{
		DMA[3].src = image + i * width;
		DMA[3].dst = videoBuffer + OFFSET(x, y + i, SCRN_WIDTH);
		DMA[3].cnt = width | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT | DMA_ON;
	}
}

// Draws a white rectangle over the sprite in the given cell
void eraseSprite(int tileX, int tileY)
{
	drawRect(tileX * TILE_SIZE, tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE, WHITE);
}


void initScreen()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;
}