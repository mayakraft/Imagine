#ifndef game_engine_h
#define game_engine_h

#include <vector>
#include <string>
#include <stdio.h>
#include <GL/glew.h>
#include <OpenGL/gl.h> // gl3.h
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "primitives.h"
#include "openGL.h"

// namespace {}

// Graphics program
GLuint gProgramID = 0;
GLint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

struct InitParams {
	unsigned int flags;
	int width;
	int height;
	const char* title;
};

struct GameEngineFixed {
	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Renderer* renderer;
	std::vector<SDL_Joystick*> controllers;
};

struct GameEngine {
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<SDL_Joystick*> controllers;
};

GameEngine initFixedFunction(InitParams params) {
	if (SDL_Init(params.flags) < 0) {
		throw std::runtime_error(SDL_GetError());
	}

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (window == NULL) { throw std::runtime_error(SDL_GetError()); }

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) { throw std::runtime_error(SDL_GetError()); }

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		throw std::runtime_error(IMG_GetError());
	}

	// Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	return GameEngine {
		.window = window,
		.renderer = renderer,
		.controllers = std::vector<SDL_Joystick*>(),
	};
}

GameEngine initProgrammable(InitParams params) {
	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	if (SDL_Init(params.flags) < 0) {
		throw std::runtime_error(SDL_GetError());
	}

	// need at least OpenGL 3.1. using 4.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (window == NULL) { throw std::runtime_error(SDL_GetError()); }

	// On MacOS, this needs to be called immediately after SDL_CreateWindow,
	// if not, any subsequent SDL method call involving window will somehow remove
	// the "SDL_WINDOW_OPENGL" flat and replace it with Metal, or something.
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) { throw std::runtime_error(SDL_GetError()); }

	// SDL_Renderer *renderer = SDL_CreateRenderer(
	// 	window,
	// 	-1,
	// 	SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// if (renderer == NULL) { throw std::runtime_error(SDL_GetError()); }

	// initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		const char* errorChar = reinterpret_cast<char const*>(glewGetErrorString(glewError));
		std::string errorString(errorChar);
		throw std::runtime_error(errorString);
	}

	// use vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		throw std::runtime_error(SDL_GetError());
	}

	// initialize OpenGL
	if (!initGL(&gProgramID, &gVertexPos2DLocation, &gVBO, &gIBO)) {
		throw std::runtime_error("Unable to initialize OpenGL");
	}

	// initialize SDL_image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		throw std::runtime_error(IMG_GetError());
	}

	// printf("gProgramID %u\n", gProgramID);
	// printf("gVertexPos2DLocation %d\n", gVertexPos2DLocation);
	// printf("gVBO %u\n", gVBO);
	// printf("gIBO %u\n", gIBO);

	return GameEngine {
		.window = window,
		// .renderer = renderer,
		.controllers = std::vector<SDL_Joystick*>(),
	};
}

// GameEngine initCopy(InitParams params) {
// 	//Initialize SDL
// 	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
// 		throw std::runtime_error(SDL_GetError());
// 	}
// 	//Use OpenGL 3.1 core
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

// 	//Create window
// 	SDL_Window* window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
// 	if(window == NULL) {
// 		throw std::runtime_error(SDL_GetError());
// 	}
// 	//Create context
// 	SDL_GLContext gContext = SDL_GL_CreateContext(window);
// 	if(gContext == NULL) {
// 		throw std::runtime_error(SDL_GetError());
// 	}

// 	//Initialize GLEW
// 	glewExperimental = GL_TRUE;
// 	GLenum glewError = glewInit();
// 	if( glewError != GLEW_OK )
// 	{
// 		printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
// 	}

// 	//Use Vsync
// 	if( SDL_GL_SetSwapInterval( 1 ) < 0 )
// 	{
// 		printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
// 	}

// 	//Initialize OpenGL
// 	if (!initGL(&gProgramID, &gVertexPos2DLocation, &gVBO, &gIBO)) {
// 		printf( "Unable to initialize OpenGL!\n" );
// 	}

// 	return GameEngine {
// 		.window = window,
// 		// .renderer = renderer,
// 		.controllers = std::vector<SDL_Joystick*>(),
// 	};
// }

SDL_Texture* loadTexture(GameEngine* engine, std::string path) {
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(engine->renderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

void dealloc(GameEngine* engine) {
	if (engine->renderer != NULL) SDL_DestroyRenderer(engine->renderer);
	if (engine->window != NULL) SDL_DestroyWindow(engine->window);
	engine->renderer = NULL;
	engine->window = NULL;
	IMG_Quit();
	SDL_Quit();
	// glDeleteProgram(gProgramID);
}

//void drawLoop(GameEngine* engine) {
//    // event will be filled with some kind of input event (if exists)
//    // https://wiki.libsdl.org/SDL2/SDL_Event
//    SDL_Event e;
//    bool quit = false;
//    while (!quit) {
//        while (SDL_PollEvent(&e)) {
//            if (e.type == SDL_QUIT) {
//                quit = true;
//            }
//        }
//        // update, draw
//        SDL_UpdateWindowSurface(engine->window);
//    }
//}

void viewportTest(GameEngine *engine, SDL_Texture *texture) {
	float SCREEN_WIDTH = 640;
	float SCREEN_HEIGHT = 480;
	//Top left corner viewport
	SDL_Rect topLeftViewport;
	topLeftViewport.x = 0;
	topLeftViewport.y = 0;
	topLeftViewport.w = SCREEN_WIDTH / 2;
	topLeftViewport.h = SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport( engine->renderer, &topLeftViewport );
	//Render texture to screen
	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
	//Bottom viewport
	SDL_Rect bottomViewport;
	bottomViewport.x = 0;
	bottomViewport.y = SCREEN_HEIGHT / 2;
	bottomViewport.w = SCREEN_WIDTH;
	bottomViewport.h = SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport( engine->renderer, &bottomViewport );
	//Render texture to screen
	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
	//Update screen
	SDL_RenderPresent( engine->renderer );
}

void render() {
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	//Bind program
	glUseProgram(gProgramID);
	//Enable vertex position
	glEnableVertexAttribArray(gVertexPos2DLocation);
	//Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	// printf("gVertexPos2DLocation %d\n", gVertexPos2DLocation);
	glVertexAttribPointer(
		gVertexPos2DLocation,
		2,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(GLfloat),
		NULL);
	//Set index data and render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);
	//Disable vertex position
	glDisableVertexAttribArray(gVertexPos2DLocation);
	//Unbind program
	glUseProgram(NULL);
}

#endif