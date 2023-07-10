#include "SDL_MAINLOOP.h"
#undef main

#define MAX_NAME  64

unsigned int displayWidth;
unsigned int displayHeight;
int width = 800;
int height = 600;
int* pixels;

unsigned int frameRate = 60;
unsigned int frameCount = 0;
float deltaTime;

int pmouseX;
int pmouseY;
int mouseX;
int mouseY;
bool isMousePressed = false;
bool isMouseDragged = false;
button mouseButton;
bool isKeyPressed = false;
bool isKeyReleased = false;
keyboard keyPressed;
keyboard keyReleased;

void (*onExit)() = NULL;
void (*onKeyPressed)(keyboard) = NULL;
void (*onKeyReleased)(keyboard) = NULL;

void updateWindowIcon();

// not accessible variables
SDL_Window* window;
SDL_Surface* surface;

bool running = false;

#ifdef MAINLOOP_GL
Uint32 winFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
#else 
Uint32 winFlags = SDL_WINDOW_SHOWN;
#endif

char windowName[MAX_NAME+1];
char iconPath[MAX_NAME+1];

#ifdef MAINLOOP_GL
SDL_Renderer* renderer = NULL;
SDL_Texture* drawBuffer = NULL;
GLuint globalShader = 0;

float render_width;
float render_height;
int win_width;
int win_height;
float render_ratio;
float win_ratio;
float localX;
float localY;
ScaleMode scale_mode = ANISOTROPIC; 

typedef struct shader_list_t {
    GLuint id;
    char* filename;
    struct shader_list_t* next;
} shader_list_t;
shader_list_t* shader_list = NULL;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;

// utility function for shader loading
GLuint compileProgram(const char*);
GLuint compileShader(const char*, GLuint);

void calculateRescaleVars();
void renderOpenGL();
int filterResize(void*, SDL_Event*);
#endif

int main(int argc, char* argv[]){
    SDL_Init(
        SDL_INIT_VIDEO |
        SDL_INIT_AUDIO
        #ifdef MAINLOOP_GL
        | SDL_VIDEO_OPENGL
        #endif
    );
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    displayWidth = displayMode.w;
    displayHeight = displayMode.h;
    strcpy(windowName, "window");
    
    #ifdef MAINLOOP_AUDIO
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    #endif
    
    setup();

    window = SDL_CreateWindow(windowName, 100, 100, width, height, winFlags);
    updateWindowIcon();

    #ifdef MAINLOOP_GL
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    drawBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	setScaleMode(scale_mode);
    SDL_SetEventFilter(filterResize, NULL);

    glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)SDL_GL_GetProcAddress("glGetShaderInfoLog");
	glDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
	glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
	glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)SDL_GL_GetProcAddress("glValidateProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)SDL_GL_GetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)SDL_GL_GetProcAddress("glGetProgramInfoLog");
	glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
	glUniform1f = (PFNGLUNIFORM1FPROC)SDL_GL_GetProcAddress("glUniform1f");
    glUniform2f = (PFNGLUNIFORM2FPROC)SDL_GL_GetProcAddress("glUniform2f");

    shader_list_t* p = shader_list;
    while(p){
        p->id = compileProgram(p->filename);
        free(p->filename);
        p = p->next;
    } 

    SDL_ShowWindow(window);
    #endif

    #ifndef MAINLOOP_GL
    surface = SDL_GetWindowSurface(window);
    pixels = (int*)surface->pixels;
    width = surface->w;
    height = surface->h;
    #endif

    Uint64 a_clock = SDL_GetPerformanceCounter();
    Uint64 b_clock = SDL_GetPerformanceCounter();
    deltaTime = 0;

    running = true;
    while(running){
        a_clock = SDL_GetPerformanceCounter();
        deltaTime = (float)(a_clock - b_clock)/SDL_GetPerformanceFrequency()*1000;

        if(deltaTime > 1000.0f / frameRate){
            frameCount++;

            #ifdef MAINLOOP_GL
            calculateRescaleVars();
            #endif

            pmouseX = mouseX;
            pmouseY = mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            #ifdef MAINLOOP_GL
            mouseX -= localX;
            mouseY -= localY;
            mouseX *= width/render_width;
            mouseY *= height/render_height;
            #endif
            if(mouseX < 0)
                mouseX = 0;
            if(mouseY < 0)
                mouseY = 0;
            if(mouseX >= width)
                mouseX = width-1;
            if(mouseY >= height)
                mouseY = height-1;
            SDL_Event event;
            isKeyReleased = false;
            isKeyPressed = false;
            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                        running = 0;
                    break;

                    case SDL_KEYDOWN:
                    keyPressed = event.key.keysym.sym;
                    isKeyPressed = true;
                    
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        running = 0;

                    if(onKeyPressed)
                        (*onKeyPressed)(keyPressed);
                    break;

                    case SDL_KEYUP:
                    isKeyReleased = true;
                    keyReleased = event.key.keysym.sym;
                    if(onKeyReleased)
                        (*onKeyReleased)(keyReleased);
                    break;

                    case SDL_MOUSEBUTTONDOWN:
                    isMousePressed = true;
                    mouseButton = event.button.button;
                    break;

                    case SDL_MOUSEBUTTONUP:
                    isMousePressed = false;
                    break;
                }
            }

            if(isMousePressed && (mouseX != pmouseX || mouseY != pmouseY))
                isMouseDragged = true;
            else
                isMouseDragged = false;

            #ifdef MAINLOOP_GL
            SDL_LockTextureToSurface(drawBuffer, NULL, &surface);
            pixels = (int*)surface->pixels;
            #endif

            loop();

            #ifdef MAINLOOP_GL
            SDL_UnlockTexture(drawBuffer);  
            renderOpenGL();
            #else 
            SDL_UpdateWindowSurface(window);
            #endif

            b_clock = a_clock;
        } else {
            float ms = 1000.0f/frameRate;
            if(ms - deltaTime > 1.0f)
                SDL_Delay(ms - deltaTime - 1);
        }
    };

    if(onExit)
        (*onExit)();

    #ifdef MAINLOOP_AUDIO
    Mix_CloseAudio();
    #endif

    #ifdef MAINLOOP_GL
    SDL_DestroyTexture(drawBuffer);
	SDL_DestroyRenderer(renderer);
    
    while(shader_list){
        shader_list_t* tmp = shader_list->next;
        free(shader_list);
        shader_list = tmp;
    }
    #endif

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void size(int w, int h){
    if(!running){
        width = w;
        height = h;
    }
}

void setTitle(const char* name){
    strncpy(windowName, name, MAX_NAME);
    if(window){
        SDL_SetWindowTitle(window, windowName);
    }
}

void setWindowIcon(const char* filename){
    strncpy(iconPath, filename, MAX_NAME);
    if(window)
        updateWindowIcon();
}

void updateWindowIcon(){
    SDL_Surface* icon = SDL_LoadBMP(iconPath);
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}

Uint64 millis(){
    return (float)SDL_GetPerformanceCounter()/SDL_GetPerformanceFrequency()*1000;
}

void fullScreen(){
    if(!(winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP))
        winFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    else 
        winFlags &= ~(SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    if(window)
        SDL_SetWindowFullscreen(window, winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP);

    #ifndef MAINLOOP_GL
    SDL_GetWindowSize(window, &width, &height);
    SDL_GetWindowSurface(window);
    pixels = (int*)surface->pixels;
    #endif
}

void background(int red, int green, int blue){
    #ifndef MAINLOOP_GL
    SDL_FillRect(surface, &surface->clip_rect, color(red, green, blue));
    #endif
}

int color(int red, int green, int blue){
    return SDL_MapRGB(surface->format, red, green, blue);
}

int red(int col){
    return col >> 16 & 0xFF;
}

int green(int col){
    return col >> 8 & 0xFF;
}

int blue(int col){
    return col & 0xFF;
}

#ifdef MAINLOOP_AUDIO

Sound* loadSound(const char* filename){
    return Mix_LoadWAV(filename);
}

void playSound(Sound* sound){
    Mix_PlayChannel(-1, sound, 0);
}

void freeSound(Sound* sound){
    Mix_FreeChunk(sound);
}

#endif

#ifdef MAINLOOP_GL
GLuint compileShader(const char* source, GLuint shaderType) {
	GLuint result = glCreateShader(shaderType);
	glShaderSource(result, 1, &source, NULL);
	glCompileShader(result);

	//Check vertex shader for errors
	GLint shaderCompiled = GL_FALSE;
	glGetShaderiv( result, GL_COMPILE_STATUS, &shaderCompiled );
	if( shaderCompiled != GL_TRUE ) {
		fprintf(stderr, "Shader compilation error %d!\n", result);
		GLint logLength;
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			GLchar *log = (GLchar*)malloc(logLength);
			glGetShaderInfoLog(result, logLength, &logLength, log);
			fprintf(stdout, "compield Shader log:\n%s\n", log);
			free(log);
		}
		glDeleteShader(result);
		result = 0;
	} else {
		printf("Shader compiled correctly. Id = %d\n", result);
	}
	return result;
}

GLuint compileProgram(const char* fragFile) {
	GLuint programId = 0;
	GLuint fragShaderId;

	programId = glCreateProgram();

	FILE* fptr = fopen(fragFile, "rb");
	fseek(fptr, 0, SEEK_END);
	size_t size = ftell(fptr);
	rewind(fptr);
	char* fcontent = (char*)malloc(size+1);
	fread(fcontent, 1, size, fptr);
    fcontent[size] = '\0';

	fragShaderId = compileShader(fcontent, GL_FRAGMENT_SHADER);
	
	if(fragShaderId) {
		glAttachShader(programId, fragShaderId);
		glLinkProgram(programId);
		glValidateProgram(programId);

		GLint logLen;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);
		if(logLen > 0) {
			char* log = (char*) malloc(logLen * sizeof(char));

			glGetProgramInfoLog(programId, logLen, &logLen, log);
			fprintf(stdout, "Prog Info Log: \n%s\n", log);
			free(log);
		}
	}
	if(fragShaderId) {
		glDeleteShader(fragShaderId);
	}
	return programId;
}

Shader loadShader(const char* filename){
    shader_list_t* block = malloc(sizeof(shader_list_t));
    block->next = NULL;
    block->filename = malloc(129);
    strncpy(block->filename, filename, 128);
    if(!shader_list){
        shader_list = block;
    } else {
        block->next = shader_list;
        shader_list = block;
    }
    return &block->id;
}

void noGlobalShader(){
    globalShader = 0;
}

void setGlobalShader(Shader sh){
    shader_list_t* p = shader_list;
    while(p->id != *sh)
        p = p->next;
    globalShader = p->id; 
}

void setScaleMode(ScaleMode mode){
    if(!renderer){
        scale_mode = mode;
        return;    
    }

    switch(mode){
        case NEAREST:
        SDL_SetTextureScaleMode(drawBuffer, SDL_ScaleModeNearest);
        break;

        case LINEAR:
        SDL_SetTextureScaleMode(drawBuffer, SDL_ScaleModeLinear);
        break;

        case ANISOTROPIC:
        SDL_SetTextureScaleMode(drawBuffer, SDL_ScaleModeBest);
        break;
    }
}

void calculateRescaleVars(){
    SDL_GetWindowSize(window, &win_width, &win_height);
    render_ratio = (float)width/height;
    win_ratio = (float)win_width/win_height;
    if(win_ratio > render_ratio){
        render_width = (float)width*win_height/height;
        render_height = win_height;
    } else {
        render_width = win_width;
        render_height = height*win_width/width;
    }
    localX = win_width/2-render_width/2;
    localY = win_height/2-render_height/2;
}

void renderOpenGL(){
    SDL_RenderClear(renderer);
    SDL_GL_BindTexture(drawBuffer, NULL, NULL);
    glUseProgram(globalShader);

    glViewport(win_width/2-render_width/2, win_height/2-render_height/2, render_width, render_height);
    glUniform1f(glGetUniformLocation(globalShader, "width"), render_width);
    glUniform1f(glGetUniformLocation(globalShader, "height"), render_height);
    glUniform1f(glGetUniformLocation(globalShader, "localX"), localX);
    glUniform1f(glGetUniformLocation(globalShader, "localY"), localY);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-1, 1);
        glTexCoord2f(1, 0); glVertex2f(1, 1);
        glTexCoord2f(1, 1); glVertex2f(1, -1);
        glTexCoord2f(0, 1); glVertex2f(-1, -1);
    glEnd();
    SDL_RenderPresent(renderer);
}

int filterResize(void* userdata, SDL_Event* event){
    if(event->type == SDL_WINDOWEVENT)
        if(event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
            calculateRescaleVars();
            renderOpenGL();
            return 0;
        }

    return 1;
}

#endif