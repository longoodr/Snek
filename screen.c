#include "screen.h"
#include "font.h"
#include "dma.h"


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
	while(SCANLINECOUNTER < 161);
}

void drawChar(int x, int y, char ch, unsigned short color)
{
	for (int xx = 0; xx < 6; xx++)
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