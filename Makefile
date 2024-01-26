gcc:
	gcc -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -lopengl32 -O2 -Wall -D MAINLOOP_GL -mwindows

debug:
	gcc -Iinclude -Llib src/*.c -lmingw32 -lSDL2main -lSDL2 -lopengl32 -Wall -D DEBUG -D MAINLOOP_GL

emcc:
	emcc -Iinclude $(wildcard src/*.c) -O2 -sUSE_SDL=2 \
	--preload-file data \
	--exclude-file data/shader \
	-sEXPORTED_FUNCTIONS="[_main,_loadGame]" \
	-sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
	-o wasm/emulator.html