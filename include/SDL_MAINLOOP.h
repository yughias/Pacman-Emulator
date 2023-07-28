#ifndef _SDL_MAINLOOP_H_
#define _SDL_MAINLOOP_H_

#include <SDL2/SDL.h>

#ifdef MAINLOOP_AUDIO
#include <SDL2/SDL_mixer.h>
#endif

#ifdef MAINLOOP_GL
#include <SDL2/SDL_opengl.h>
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define frameRate(x) frameRate = x

#define LEFT SDLK_LEFT
#define RIGHT SDLK_RIGHT
#define UP SDLK_UP
#define DOWN SDLK_DOWN
#define BUTTON_LEFT SDL_BUTTON_LEFT
#define BUTTON_RIGHT SDL_BUTTON_RIGHT

typedef int keyboard;
typedef Uint32 button;

extern unsigned int displayWidth;
extern unsigned int displayHeight;
extern int width;
extern int height;
extern int* pixels;

extern unsigned int frameRate;
extern unsigned int frameCount;
extern float deltaTime;

extern int pmouseX;
extern int pmouseY;
extern int mouseX;
extern int mouseY;
extern bool isMousePressed;
extern bool isMouseDragged;
extern button mouseButton;
extern bool isKeyPressed;
extern bool isKeyReleased;
extern keyboard keyPressed;
extern keyboard keyReleased;

void setup();
void loop();
void size(int, int);
void setTitle(const char*);
void setWindowIcon(const char*);
Uint64 millis();
void fullScreen();
void background(int, int, int);
int color(int, int, int);
int red(int);
int green(int);
int blue(int);

// Custom callbacks
extern void (*onExit)();
extern void (*onKeyPressed)(keyboard);
extern void (*onKeyReleased)(keyboard);

#ifdef MAINLOOP_AUDIO
typedef Mix_Chunk Sound;

// Audio function 
Sound* loadSound(const char*);
void playSound(Sound*);
void freeSound(Sound*);
#endif

// define MAINLOOP_GL variables and functions, if symbol does not exit function are transformed to empty macros
typedef enum {NEAREST, LINEAR, ANISOTROPIC} ScaleMode;
#ifdef MAINLOOP_GL
typedef GLuint* Shader;
Shader loadShader(const char*);
void noGlobalShader();
void setGlobalShader(Shader);
void setScaleMode(ScaleMode);
#else
typedef int Shader;
#define loadShader(x) 0;
#define noGlobalShader();
#define setGlobalShader(x);
#define setScaleMode(x);
#endif

#endif