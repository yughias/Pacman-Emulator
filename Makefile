all:
	gcc -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -lopengl32 -O2 -Wall -D MAINLOOP_GL
debug:
	gcc -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -lopengl32 -Wall -D DEBUG -D MAINLOOP_GL