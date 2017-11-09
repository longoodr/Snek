#ifndef SCREEN_H
#define SCREEN_H
void drawRect(int x, int y, int height, int width, unsigned short color);
void delay(int n);
void waitForVblank();
void drawSprite(int tileX, int tileY, const unsigned short* sprite);
void drawImage3(int x, int y, int width, int height, const unsigned short* image);
void drawChar(int x, int y, char ch, unsigned short color);
void drawString(int x, int y, char* string, unsigned short color);
void eraseSprite(int tileX, int tileY);
void initScreen();

extern unsigned short* videoBuffer;

#define REG_DISPCTL *(unsigned short *)0x4000000
#define MODE3 3

#define SCANLINECOUNTER *(volatile unsigned short *) 0x4000006

#define BG2_ENABLE (1<<10)

#define COLOR(r, g, b) ((r) | (g)<<5 | (b)<<10)

#define SET_PIXEL(x, y, color) (videoBuffer[OFFSET((x), (y), SCRN_WIDTH)] = (color))

#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define WHITE COLOR(31, 31, 31)
#define YELLOW COLOR(31, 31,0)
#define OFFSET(x, y, xLength) ((y) * (xLength) + x)
#define BLACK 0

#define SCRN_HEIGHT 160
#define SCRN_WIDTH 240

#define TILE_SIZE 10
#define TILE_Y 14
#define TILE_X 24
#endif