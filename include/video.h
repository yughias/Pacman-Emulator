#ifndef __VIDEO_H__
#define __VIDEO_H__

#define VIDEO_COLS 28
#define VIDEO_ROWS 36

void initVideoData();
void freeVideoData();
int getColorFromROM(uint8_t);
void getPaletteFromROM(uint8_t, int*);
void getTileFromROM(uint8_t, uint8_t, int*);
void getSpriteFromROM(uint8_t, uint8_t, int*);
void drawTile(int, int, int*);
void drawSprite(int, int, uint8_t, uint8_t, bool, bool);
void drawVideo();


#endif