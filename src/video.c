#include <hardware.h>
#include <SDL_MAINLOOP.h>

uint8_t* colorROM;
uint8_t* paletteROM;
uint8_t* tileROM;
uint8_t* spriteROM;

uint8_t* palettesInfo;
uint8_t* tilesInfo;
uint8_t* spritesInfo;

uint8_t colorTable[8] = { 
/* RED   BIT 0 */         0x21,
/* RED   BIT 1 */         0x47,
/* RED   BIT 2 */         0x97,
/* GREEN BIT 0 */         0x21,
/* GREEN BIT 1 */         0x47,
/* GREEN BIT 2 */         0x97,
/* BLUE  BIT 0 */         0x51,
/* BLUE  BIT 1 */         0xAE
                        };

void initVideoData(){
    colorROM   = malloc(32);
    paletteROM = malloc(256);
    tileROM    = malloc(4096);
    spriteROM  = malloc(4096);
    loadROM("data/ROM/82s123.7f", 32,   colorROM);
    loadROM("data/ROM/82s126.4a", 256,  paletteROM);
    loadROM("data/ROM/pacman.5e", 4096, tileROM);
    loadROM("data/ROM/pacman.5f", 4096, spriteROM);

    palettesInfo = MEMORY + PALETTE_RAM;
    tilesInfo    = MEMORY + TILE_RAM; 
    spritesInfo  = MEMORY + SPRITE_RAM;
}

void freeVideoData(){
    free(colorROM);
    free(paletteROM);
    free(tileROM);
    free(spriteROM);
}

int getColorFromROM(uint8_t idx){
    uint8_t colorPacman = colorROM[idx];
    int colorRGB = 255 << 24;
    
    uint8_t red = 0;
    for(int i = 0; i < 3; i++){
        bool bit = colorPacman & 0x1;
        red += bit*colorTable[i];
        colorPacman >>= 1;
    } 
    colorRGB |= red << 16;

    uint8_t green = 0;
    for(int i = 0; i < 3; i++){
        bool bit = colorPacman & 0x1;
        green += bit*colorTable[3+i];
        colorPacman >>= 1;
    }  
    colorRGB |= green << 8;

    uint8_t blue = 0;
    for(int i = 0; i < 2; i++){
        bool bit = colorPacman & 0x1;
        blue += bit*colorTable[6+i];
        colorPacman >>= 1;
    }  
    colorRGB |= blue;
    return colorRGB;
}

void getPaletteFromROM(uint8_t idx, int* paletteRGB){
    idx &= 0x1F;
    for(int i = 0; i < 4; i++)
        paletteRGB[i] = getColorFromROM(paletteROM[(idx*4)+i]);
}

void getTileFromROM(uint8_t tileIdx, uint8_t paletteIdx, int* tileRGB){
    int paletteRGB[4];

    getPaletteFromROM(paletteIdx, paletteRGB);
    for(int i = 0; i < 16; i++){
        uint8_t byte = tileROM[tileIdx*16+i];
        for(int j = 0; j < 4; j++){
            uint8_t mask = 1 << j;
            uint8_t paletteColor = 0;
            paletteColor |= (bool)(byte & mask);
            paletteColor |= (bool)(byte & (mask << 4)) << 1;
            uint8_t offX = 7-(i % 8);
            uint8_t offY = i < 8 ? 4+(3-j) : 3-j;
            tileRGB[offX+offY*8] = paletteRGB[paletteColor];
        }
    }
}

void getSpriteFromROM(uint8_t spriteIdx, uint8_t paletteIdx, int* spriteRGB){
    int paletteRGB[4];

    getPaletteFromROM(paletteIdx, paletteRGB);
    for(int i = 0; i < 64; i++){
        uint8_t byte = spriteROM[spriteIdx*64+i];
        for(int j = 0; j < 4; j++){
            uint8_t mask = 1 << j;
            uint8_t paletteColor = 0;
            paletteColor |= (bool)(byte & mask);
            paletteColor |= (bool)(byte & (mask << 4)) << 1;
            uint8_t offX = i < 32 ? 15-(i%8) : 7-(i%8);
            uint8_t offY = 0;
            
            if(i < 8)
                offY = 12+(3-j);
            else if(i < 16)
                offY = 3-j;
            else if(i < 24)
                offY = 4+(3-j);
            else if(i < 32)
                offY = 8+(3-j);
            else if(i < 40)
                offY = 12+(3-j);
            else if(i < 48)
                offY = (3-j);
            else if(i < 56)
                offY = 4+(3-j);
            else
                offY = 8+(3-j);
            
            spriteRGB[offX+offY*16] = paletteRGB[paletteColor];
        }
    }
}

void drawTile(int offX, int offY, int* tile){
    offX *= 8;
    offY *= 8;
    for(int y = 0; y < 8; y++)
        for(int x = 0; x < 8; x++)
            pixels[(offX+x)+(offY+y)*width] = tile[x+y*8];
}

void drawSprite(int offX, int offY, uint8_t spriteIdx, uint8_t paletteIdx, bool flipX, bool flipY){
    int spriteRGB[256];
    int paletteRGB[4];
    getPaletteFromROM(paletteIdx, paletteRGB);
    getSpriteFromROM(spriteIdx, paletteIdx, spriteRGB);
    
    int spriteX;
    int spriteY;
    int stepX = flipX ? -1 : 1;
    int stepY = flipY ? -1 : 1;

    spriteY = flipY ? 15 : 0;
    for(int y = 0; y < 16; y++){
        spriteX = flipX ? 15 : 0;
        for(int x = 0; x < 16; x++){
            int offSprite = spriteX + spriteY*16;
            if(spriteRGB[offSprite] != paletteRGB[0]){
                int screenX = offX+x;
                int screenY = (offY+y) % height;
                if(screenX < width && screenY >= 16 && screenY < height - 16)
                    pixels[screenX+screenY*width] = spriteRGB[offSprite];
            }
            spriteX += stepX;
        }
        spriteY += stepY;
    }
}

void drawVideo(){
    // draw bottom screen
    for(int y = 0; y < 2; y++)
        for(int x = 0; x < VIDEO_COLS; x++){
            int data = 0x02 + x + y*0x20;
            int tile[64];
            getTileFromROM(tilesInfo[data], palettesInfo[data], tile);
            drawTile(VIDEO_COLS-1-x, VIDEO_ROWS-2+y, tile);
        }
    
    // draw top screen
    for(int y = 0; y < 2; y++)
        for(int x = 0; x < VIDEO_COLS; x++){
            int data = 0x3C2 + x + y*0x20;
            int tile[64];
            getTileFromROM(tilesInfo[data], palettesInfo[data], tile);
            drawTile(VIDEO_COLS-1-x, y, tile);
        }

    // draw central screen
    for(int y = 0; y < 32; y++)
        for(int x = 0; x < VIDEO_COLS; x++){
            int data = 0x40 + y + x*0x20;
            int tile[64];
            getTileFromROM(tilesInfo[data], palettesInfo[data], tile);
            drawTile(VIDEO_COLS-1-x, 2+y, tile);
        }

    // draw sprites
    for(int i = 7; i >= 0; i--){
        uint8_t paletteIdx = spritesInfo[i*2+1];
        uint8_t spriteIdx  = (spritesInfo[i*2] & 0xFC) >> 2;
        bool flipX = spritesInfo[i*2] & 0x2;
        bool flipY = spritesInfo[i*2] & 0x1;
        int coordX = width - SPRITE_COORDS[i*2] + 15;
        int coordY = height - SPRITE_COORDS[i*2+1] - 16;
        drawSprite(coordX, coordY, spriteIdx, paletteIdx, flipX, flipY);
    }
}