all:
	gcc -g -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -O2 -Wall
debug:
	gcc -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -Wall -D DEBUG