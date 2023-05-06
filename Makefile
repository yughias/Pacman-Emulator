all:
	gcc -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -O2 -Wall
debug:
	gcc -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -D DEBUG