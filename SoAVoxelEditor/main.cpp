#pragma once
#define _CRT_SECURE_NO_WARNINGS
//
// main.cpp
//
// Copyright 2014 Seed Of Andromeda
//

#ifdef _WIN32
#include <Windows.h>
#endif
#include <SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <thread>
#include "Options.h"
#include "Errors.h"
#include "Camera.h"
#include "FileManager.h"
#include "Shader.h"
#include "GlobalStructs.h"
#include "RenderUtil.h"
#include "VoxelEditor.h"
#include "TextureManager.h"
#include "Voxel.h"
#include "Awesomium.h"

#include <stdlib.h>
#include <time.h> 

void initialize();
void initializeSdlOpengl();
void initializeShaders();
void control();
void update();
void draw();
void destroy();
bool checkGlError();

enum states{INITIALIZE, PLAY, EXIT};

int gameState;

SDL_Window *mainWindow;
SDL_GLContext mainOpenGLContext;

Camera *mainCamera;

VoxelEditor voxelEditor;

int main(int argc, char **argv)
{
    srand(time(NULL));
	initialize();

	while (gameState == PLAY){
		if (RenderUtil::checkGlError()){
			break;
		}

		control();
		update();
		draw();
		
		//TODO: FPS limiter
	}

    RenderUtil::releaseWireframeBox();

	//save options in case they changed.
	saveOptions("Data/options.ini");
	destroy();
	return 0;
}

void initialize()
{
	gameState = INITIALIZE;
	fileManager.initialize();

    //Initialize this first so that all future opengl calls are successful
    initializeSdlOpengl();

	loadOptions("Data/options.ini");

    voxelEditor.initialize();

	initializeShaders();
	
    TextureManager::loadTextures();

    drawDebugLine = false;

	mainCamera = new Camera();

	gameState = PLAY;
}

void initializeSdlOpengl()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	cout << "Video Driver: " << SDL_GetCurrentVideoDriver() << endl;
	SDL_DisplayMode mode;
	SDL_GetCurrentDisplayMode(0, &mode);
	cout << "Window Info - W: " << mode.w << " H: " << mode.h << " Refresh: " << mode.refresh_rate << " " << endl;
	graphicsOptions.nativeWidth = mode.w;
	graphicsOptions.nativeHeight = mode.h;


	Uint32 wflags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	if (graphicsOptions.isFullscreen) wflags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	if (graphicsOptions.isBorderless) wflags |= SDL_WINDOW_BORDERLESS;
	mainWindow = SDL_CreateWindow("SoA Voxel Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, graphicsOptions.screenWidth, graphicsOptions.screenHeight, wflags);

	if (mainWindow == NULL){
		exit(343);
	}

	mainOpenGLContext = SDL_GL_CreateContext(mainWindow);
	if (mainOpenGLContext == NULL){
		error("Could not make openGL context!");
		SDL_Quit();
	}
	SDL_GL_MakeCurrent(mainWindow, mainOpenGLContext);

	SDL_GL_SetSwapInterval((int)graphicsOptions.isVsync);

	GLenum err = glewInit();
	if (err != GLEW_OK){
		int in;
		error("Glew failed to initialize. Your graphics card is probably WAY too old. Or you forgot to extract the .zip. It might be time for an upgrade :)");
		exit(133);
	}

	int vminor;
	int vmajor;
	glGetIntegerv(GL_MAJOR_VERSION, &vmajor);
	glGetIntegerv(GL_MINOR_VERSION, &vminor);
	printf("\n***		Opengl Version: %s\n", glGetString(GL_VERSION));
	printf("\n***       CPU Threads: %u\n", thread::hardware_concurrency());

	//check that opengl versio is at least 3
	if (vmajor < 3){// I dont think its works like I think it does
		char buffer[2048];
		sprintf(buffer, "Your graphics card driver does not support at least OpenGL 3.3. Your OpenGL version is \"%s\". The game will most likely not work.\n\nEither your graphics card drivers are not up to date, or your computer is using an integrated graphics card instead of your gaming card.\nYou should be able to switch to your main graphics card by right clicking your desktop and going to graphics properties.", glGetString(GL_VERSION));
		error(buffer);
		//		exit(133);
	}

	if (!GLEW_VERSION_2_1){  // check that the machine supports the 2.1 API.
		int in;
		error("Machine does not support 2.1 GLEW API.");
		exit(134);
	}

	int textureunits;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureunits);
	if (textureunits < 8){
		error("Your graphics card does not support at least 8 texture units! It only supports " + to_string(textureunits));
		exit(1);
	}

	glClearColor(0 / 255.0f, 0 / 255.0f, 255.0f / 255.0f, 1.0f); //blue background
	glLineWidth(1);

	//Alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glDisable(GL_MULTISAMPLE);

	//TODO: Ben, do these things
	//initializeText2D("Fonts/OrbitronBold.png", "Fonts/FontData.dat");
	//initializeTTF();
	//initializeFrameBuffer(); //initialize our FBO
}


void initializeShaders() {
	blockShader.initialize("Shaders/");
	gridShader.initialize("Shaders/");
    wireframeShader.initialize("Shaders/");
}


//get the SDL user input
void control() {
	SDL_Event evnt;
	Voxel v;

	while (SDL_PollEvent(&evnt)) {
		if (voxelEditor.getState() == 'i'){
			voxelEditor.findIntersect(mainCamera->getPosition(), mainCamera->screenToWorld(glm::vec2(evnt.motion.x, evnt.motion.y), graphicsOptions.screenWidth, graphicsOptions.screenHeight));
		}
		switch (evnt.type) {
		case SDL_QUIT:
			gameState = EXIT;
			return;
		case SDL_MOUSEMOTION:
			if (MouseButtons[SDL_BUTTON_RIGHT]){
				mainCamera->mouseMove(evnt.motion.xrel, evnt.motion.yrel);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
            if(evnt.button.button == SDL_BUTTON_LEFT) {
				if (voxelEditor.getState() != 'i'){
					voxelEditor.findIntersect(mainCamera->getPosition(), mainCamera->screenToWorld(glm::vec2(evnt.motion.x, evnt.motion.y), graphicsOptions.screenWidth, graphicsOptions.screenHeight));
				}
				voxelEditor.handleClick();
			}
            MouseButtons[evnt.button.button] = true;
			break;
		case SDL_MOUSEBUTTONUP:
            std::cout << "Mouse button released!" << std::endl;
			MouseButtons[evnt.button.button] = false;
			break;
		case SDL_MOUSEWHEEL:
			mainCamera->mouseZoom(evnt.wheel.y);
			break;
		case SDL_KEYDOWN:
			Keys[evnt.key.keysym.sym].pr = true;
			switch (evnt.key.keysym.sym){
			case SDLK_t:
                voxelEditor.cycleState();
				break;
			case SDLK_z:
				if (Keys[SDLK_LCTRL].pr == 1){
					printf("undo called\n");
					voxelEditor.undo();
				}
				break;
			case SDLK_y:
				if (Keys[SDLK_LCTRL].pr == 1){
					printf("redo called\n");
					voxelEditor.redo();
				}
				break;
            case SDLK_f:
                voxelEditor.fillSelected();
                break;
            case SDLK_r:
                voxelEditor.removeSelected();
                break;
            case SDLK_p:
               for(int i = 0; i < 3; i++)
                    v.color[i] = rand() % 256;
                v.color[3] = 255;
                v.type = 'b';
                voxelEditor.setCurrentVoxel(v);
                break;
			case SDLK_c:
				v.type = 'b';
				int color[4];
				cout << "Red: ";
				cin >> color[0];
				cout << "Green: ";
				cin >> color[1];
				cout << "Blue: ";
				cin >> color[2];
				cout << "Alpha: ";
				cin >> color[3];
				//printf("current voxel color input format: r g b a\n");
				//scanf("%d %d %d %d", color[0], color[1], color[2], color[3]);
				v.color[0] = color[0];
				v.color[1] = color[1];
				v.color[2] = color[2];
				v.color[3] = color[3];
				cout << "Color inputs are " << v.color[0] << " " << v.color[1] << " " << v.color[2] << endl;
				voxelEditor.setCurrentVoxel(v);
				break;
			case SDLK_x:
				v = *voxelEditor.getCurrentVoxel();
				cout << "_currentVoxel color: " << v.color[0] << " " << v.color[1] << " " << v.color[2] << " " << v.color[3] << endl;
				break;
			case SDLK_b:
				voxelEditor.brushRange();
				break;
            }
			break;

		case SDL_KEYUP:
			Keys[evnt.key.keysym.sym].pr = false;
			break;
		}
	}
}

void update() {
    voxelEditor.update();
	mainCamera->update();
}

//draws the scene
void draw() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    voxelEditor.draw(mainCamera);

    SDL_GL_SwapWindow(mainWindow);
}

void destroy() {
	//this stuff is technically optional, since it should happen on app exit anyways
	SDL_GL_DeleteContext(mainOpenGLContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
}