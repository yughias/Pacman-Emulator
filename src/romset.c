#include <romset.h>
#include <hardware.h>

RomSet pacman = {
    .name       = "pacman",
    .initMemory = initMemory_pacman,
    .initVideo  = initVideo_pacman
};


RomSet mspacman = {
    .name       = "mspacman",
    .initMemory = initMemory_mspacman,
    .initVideo  = initVideo_mspacman
};

RomSet pacmanf = {
    .name = "pacmanf",
    .initMemory = initMemory_pacmanf,
    .initVideo  = initVideo_pacman,
};

RomSet mspacmnf = {
    .name = "mspacmnf",
    .initMemory = initMemory_mspacmnf,
    .initVideo  = initVideo_mspacman
};

RomSet pacmod = {
    .name = "pacmod",
    .initMemory = initMemory_pacmod,
    .initVideo  = initVideo_pacmod 
};

RomSet pacplus = {
    .name = "pacplus",
    .initMemory = initMemory_pacplus,
    .initVideo  = initVideo_pacplus
};

RomSet* romsetArray[] = { &pacman, &mspacman, &pacmanf, &mspacmnf, &pacmod, &pacplus };
int romsetArrayLength = sizeof(romsetArray)/sizeof(romsetArray[0]);
int currentRom = 0;
char romPath[] = "data/ROM";

void loadROM(const char* romname, size_t size, uint8_t* buffer){
    char filename[128];
    snprintf(filename, 127, "%s/%s/%s", romPath, romsetArray[currentRom]->name, romname);
    FILE* fptr = fopen(filename, "rb");
    fread(buffer, 1, size, fptr);
    fclose(fptr);
}

// decrypt functions and utilities
uint16_t decryptAddr1(uint16_t);
uint16_t decryptAddr2(uint16_t);
uint8_t decryptByte(uint8_t);
uint8_t swapByte(uint8_t, const uint8_t*);
uint8_t decryptPacplus(uint16_t, uint8_t);
void initAuxBoard_mspacman();

// init functions
void initMemory_pacman(){
    ROM = malloc(ROM_SIZE);
    RAM = malloc(RAM_SIZE);
    memset(IO, 0, IO_SIZE);

    loadROM("pacman.6e", 0x1000, ROM       );
    loadROM("pacman.6f", 0x1000, ROM+0x1000);
    loadROM("pacman.6h", 0x1000, ROM+0x2000);
    loadROM("pacman.6j", 0x1000, ROM+0x3000);
    
    AUX_INSTALLED = false;
    AUX_ROM_LOW = NULL;
    AUX_ROM_HIGH = NULL;
}

void initMemory_mspacman(){
    initMemory_pacman();
    initAuxBoard_mspacman();
}

void initMemory_pacmanf(){
    ROM = malloc(ROM_SIZE);
    RAM = malloc(RAM_SIZE);
    memset(IO, 0, IO_SIZE);

    loadROM("pacman.6e", 0x1000, ROM       );
    loadROM("pacfast.6f", 0x1000, ROM+0x1000);
    loadROM("pacman.6h", 0x1000, ROM+0x2000);
    loadROM("pacman.6j", 0x1000, ROM+0x3000);
    
    AUX_INSTALLED = false;
    AUX_ROM_LOW = NULL;
    AUX_ROM_HIGH = NULL;
}

void initMemory_mspacmnf(){
    initMemory_pacmanf();
    initAuxBoard_mspacman();
}

void initMemory_pacmod(){
    ROM = malloc(ROM_SIZE);
    RAM = malloc(RAM_SIZE);
    memset(IO, 0, IO_SIZE);

    loadROM("pacmanh.6e", 0x1000, ROM       );
    loadROM("pacman.6f", 0x1000, ROM+0x1000);
    loadROM("pacmanh.6h", 0x1000, ROM+0x2000);
    loadROM("pacmanh.6j", 0x1000, ROM+0x3000);
    
    AUX_INSTALLED = false;
    AUX_ROM_LOW = NULL;
    AUX_ROM_HIGH = NULL;
}

void initMemory_pacplus(){
    ROM = malloc(ROM_SIZE);
    RAM = malloc(RAM_SIZE);
    memset(IO, 0, IO_SIZE);

    loadROM("pacplus.6e", 0x1000, ROM       );
    loadROM("pacplus.6f", 0x1000, ROM+0x1000);
    loadROM("pacplus.6h", 0x1000, ROM+0x2000);
    loadROM("pacplus.6j", 0x1000, ROM+0x3000);
    
    AUX_INSTALLED = false;
    AUX_ROM_LOW = NULL;
    AUX_ROM_HIGH = NULL;

    for(int i = 0; i < ROM_SIZE; i++)
        ROM[i] = decryptPacplus(i, ROM[i]);
}

void initVideo_pacman(){
    colorROM   = malloc(32);
    paletteROM = malloc(256);
    tileROM    = malloc(4096);
    spriteROM  = malloc(4096);
    loadROM("82s123.7f", 32,   colorROM);
    loadROM("82s126.4a", 256,  paletteROM);
    loadROM("pacman.5e", 4096, tileROM);
    loadROM("pacman.5f", 4096, spriteROM);
}

void initVideo_mspacman(){
    colorROM   = malloc(32);
    paletteROM = malloc(256);
    tileROM    = malloc(4096);
    spriteROM  = malloc(4096);
    loadROM("82s123.7f", 32,   colorROM);
    loadROM("82s126.4a", 256,  paletteROM);
    loadROM("5e", 4096, tileROM);
    loadROM("5f", 4096, spriteROM);
}

void initVideo_pacmod(){
    colorROM   = malloc(32);
    paletteROM = malloc(256);
    tileROM    = malloc(4096);
    spriteROM  = malloc(4096);
    loadROM("82s123.7f", 32,   colorROM);
    loadROM("82s126.4a", 256,  paletteROM);
    loadROM("pacmanh.5e", 4096, tileROM);
    loadROM("pacman.5f", 4096, spriteROM);
}

void initVideo_pacplus(){
    colorROM   = malloc(32);
    paletteROM = malloc(256);
    tileROM    = malloc(4096);
    spriteROM  = malloc(4096);
    loadROM("pacplus.7f", 32,   colorROM);
    loadROM("pacplus.4a", 256,  paletteROM);
    loadROM("pacplus.5e", 4096, tileROM);
    loadROM("pacplus.5f", 4096, spriteROM);
}

// decrypt functions implementation
uint16_t decryptAddr1(uint16_t val){
    uint16_t out;

    out  = (val & 0x807);
    out = out | (val & 0x400) >> 7;
    out = out | (val & 0x200) >> 2;
    out = out | (val & 0x080) << 3;
    out = out | (val & 0x040) << 2;
    out = out | (val & 0x138) << 1;

    return out;
}

uint16_t decryptAddr2(uint16_t val){
    uint16_t out;

    out  = (val & 0x807);
    out = out | (val & 0x040) << 4;
    out = out | (val & 0x100) >> 3;
    out = out | (val & 0x080) << 2;
    out = out | (val & 0x600) >> 2;
    out = out | (val & 0x028) << 1;
    out = out | (val & 0x010) >> 1;

    return out;
}

uint8_t decryptByte(uint8_t val){
    uint8_t out;

    out  = (val & 0xC0) >> 3;
    out = out | (val & 0x10) << 2;
    out = out | (val & 0x0E) >> 1;
    out = out | (val & 0x01) << 7;
    out = out | (val & 0x20);

    return out;
}

uint8_t swapByte(uint8_t byte, const uint8_t* index){
    uint8_t out = 0;
    for(int i = 0; i < 8; i++){
        bool bit = byte & (1 << index[i]);
        out |= (bit << (7-i));
    }
    return out;
}

// decoding code at MAME source code: 
// https://github.com/mamedev/mame/blob/master/src/mame/pacman/pacplus.cpp
uint8_t decryptPacplus(uint16_t addr, uint8_t byte){
    static const uint8_t swap_xor_table[6][9] =
	{
		{ 7,6,5,4,3,2,1,0, 0x00 },
		{ 7,6,5,4,3,2,1,0, 0x28 },
		{ 6,1,3,2,5,7,0,4, 0x96 },
		{ 6,1,5,2,3,7,0,4, 0xbe },
		{ 0,3,7,6,4,2,1,5, 0xd5 },
		{ 0,3,4,6,7,2,1,5, 0xdd }
	};

	static const int picktable[32] =
	{
		0,2,4,2,4,0,4,2,2,0,2,2,4,0,4,2,
		2,2,4,0,4,2,4,0,0,4,0,4,4,2,4,2
	};
	uint32_t method = 0;
	const uint8_t *tbl;

	/* pick method from bits 0 2 5 7 9 of the address */
	method = picktable[
		(addr & 0x001) |
		((addr & 0x004) >> 1) |
		((addr & 0x020) >> 3) |
		((addr & 0x080) >> 4) |
		((addr & 0x200) >> 5)];

	/* switch method if bit 11 of the address is set */
	if ((addr & 0x800) == 0x800)
		method ^= 1;

	tbl = swap_xor_table[method];
	return swapByte(byte, tbl) ^ tbl[8];
}

void initAuxBoard_mspacman(){
    AUX_INSTALLED = true;
    AUX_ROM_LOW = malloc(ROM_SIZE);
    AUX_ROM_HIGH = malloc(0x1800);

    
    uint8_t* U6 = malloc(0x1000);
    uint8_t* U7 = malloc(0x1000);
    uint8_t* U5 = malloc(0x0800);

    loadROM("u6",        0x1000, U6);
    loadROM("u7",        0x1000, U7);
    loadROM("u5",        0x0800, U5);

    memcpy(AUX_ROM_LOW, ROM, 0x3000);

    for(uint16_t i = 0; i < 0x1000; i++){
        AUX_ROM_LOW[0x3000+decryptAddr1(i)]  = decryptByte(U7[i]);
        AUX_ROM_HIGH[0x800+decryptAddr1(i)] = decryptByte(U6[i]);
    }

    for(uint16_t i = 0; i < 0x0800; i++)
        AUX_ROM_HIGH[decryptAddr2(i)] = decryptByte(U5[i]);

    for(int i = 0; i < 8; i++){
        AUX_ROM_LOW[0x0410+i] = AUX_ROM_HIGH[0x0008+i];
        AUX_ROM_LOW[0x08E0+i] = AUX_ROM_HIGH[0x01D8+i];
        AUX_ROM_LOW[0x0A30+i] = AUX_ROM_HIGH[0x0118+i];
        AUX_ROM_LOW[0x0BD0+i] = AUX_ROM_HIGH[0x00D8+i];
        AUX_ROM_LOW[0x0C20+i] = AUX_ROM_HIGH[0x0120+i];
        AUX_ROM_LOW[0x0E58+i] = AUX_ROM_HIGH[0x0168+i];
        AUX_ROM_LOW[0x0EA8+i] = AUX_ROM_HIGH[0x0198+i];

        AUX_ROM_LOW[0x1000+i] = AUX_ROM_HIGH[0x0020+i];
        AUX_ROM_LOW[0x1008+i] = AUX_ROM_HIGH[0x0010+i];
        AUX_ROM_LOW[0x1288+i] = AUX_ROM_HIGH[0x0098+i];
        AUX_ROM_LOW[0x1348+i] = AUX_ROM_HIGH[0x0048+i];
        AUX_ROM_LOW[0x1688+i] = AUX_ROM_HIGH[0x0088+i];
        AUX_ROM_LOW[0x16B0+i] = AUX_ROM_HIGH[0x0188+i];
        AUX_ROM_LOW[0x16D8+i] = AUX_ROM_HIGH[0x00C8+i];
        AUX_ROM_LOW[0x16F8+i] = AUX_ROM_HIGH[0x01C8+i];
        AUX_ROM_LOW[0x19A8+i] = AUX_ROM_HIGH[0x00A8+i];
        AUX_ROM_LOW[0x19B8+i] = AUX_ROM_HIGH[0x01A8+i];

        AUX_ROM_LOW[0x2060+i] = AUX_ROM_HIGH[0x0148+i];
        AUX_ROM_LOW[0x2108+i] = AUX_ROM_HIGH[0x0018+i];
        AUX_ROM_LOW[0x21A0+i] = AUX_ROM_HIGH[0x01A0+i];
        AUX_ROM_LOW[0x2298+i] = AUX_ROM_HIGH[0x00A0+i];
        AUX_ROM_LOW[0x23E0+i] = AUX_ROM_HIGH[0x00E8+i];
        AUX_ROM_LOW[0x2418+i] = AUX_ROM_HIGH[0x0000+i];
        AUX_ROM_LOW[0x2448+i] = AUX_ROM_HIGH[0x0058+i];
        AUX_ROM_LOW[0x2470+i] = AUX_ROM_HIGH[0x0140+i];
        AUX_ROM_LOW[0x2488+i] = AUX_ROM_HIGH[0x0080+i];
        AUX_ROM_LOW[0x24B0+i] = AUX_ROM_HIGH[0x0180+i];
        AUX_ROM_LOW[0x24D8+i] = AUX_ROM_HIGH[0x00C0+i];
        AUX_ROM_LOW[0x24F8+i] = AUX_ROM_HIGH[0x01C0+i];
        AUX_ROM_LOW[0x2748+i] = AUX_ROM_HIGH[0x0050+i];
        AUX_ROM_LOW[0x2780+i] = AUX_ROM_HIGH[0x0090+i];
        AUX_ROM_LOW[0x27B8+i] = AUX_ROM_HIGH[0x0190+i];
        AUX_ROM_LOW[0x2800+i] = AUX_ROM_HIGH[0x0028+i];
        AUX_ROM_LOW[0x2B20+i] = AUX_ROM_HIGH[0x0100+i];
        AUX_ROM_LOW[0x2B30+i] = AUX_ROM_HIGH[0x0110+i];
        AUX_ROM_LOW[0x2BF0+i] = AUX_ROM_HIGH[0x01D0+i];
        AUX_ROM_LOW[0x2CC0+i] = AUX_ROM_HIGH[0x00D0+i];
        AUX_ROM_LOW[0x2CD8+i] = AUX_ROM_HIGH[0x00E0+i];
        AUX_ROM_LOW[0x2CF0+i] = AUX_ROM_HIGH[0x01E0+i];
        AUX_ROM_LOW[0x2D60+i] = AUX_ROM_HIGH[0x0160+i];
    }

    free(U6);
    free(U7);
    free(U5);
}