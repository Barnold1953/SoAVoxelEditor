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

void initialize();
void initializeSdlOpengl();
void initializeVertexBuffer();
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

vector <BlockVertex> currentVerts;
//vector <GLuint> currentIndices;
GLuint *currentIndices;
BlockMesh baseMesh;
bool changed = true;
char state = 's';

//vector <texture*> cubeTexts;
//vector <texture*> cubeSelectedTexts;

VoxelEditor voxelEditor;


int main(int argc, char **argv)
{
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

	//save options in case they changed.
	saveOptions("Data/options.ini");
	destroy();
	return 0;
}



void initialize()
{
	gameState = INITIALIZE;
	fileManager.initialize();

	loadOptions("Data/options.ini");

    voxelEditor.initialize();

	initializeSdlOpengl();
	initializeVertexBuffer();
	initializeShaders();
	
    TextureManager::loadTextures();

    drawDebugLine = 0;

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

void initializeVertexBuffer(){
	int w, h, l;
	w = gameGrid->w;
	h = gameGrid->h;
	l = gameGrid->l;
	int cubeTot = w*h*l;

	currentVerts.reserve(24 * cubeTot);
	currentIndices = new GLuint[w * h * l * 36];

	int j = 0;
	for (int i = 0; i < w*h*l * 36; i+=6){
		currentIndices[i] = j;
		currentIndices[i+1] = j+1;
		currentIndices[i+2] = j+2;
		currentIndices[i+3] = j+2;
		currentIndices[i+4] = j+3;
		currentIndices[i+5] = j;
		j += 4;
	}
	
	for (int i = 0; i < 24; i++){
		baseMesh.verts[i].position.x = cubeVertices[i * 3];
		baseMesh.verts[i].position.y = cubeVertices[i * 3 + 1];
		baseMesh.verts[i].position.z = cubeVertices[i * 3 + 2];

		baseMesh.verts[i].normal.x = cubeNormals[i * 3];
		baseMesh.verts[i].normal.y = cubeNormals[i * 3 + 1];
		baseMesh.verts[i].normal.z = cubeNormals[i * 3 + 2];

		baseMesh.verts[i].color[0] = 255;
		baseMesh.verts[i].color[1] = 0;
		baseMesh.verts[i].color[2] = 0;
		baseMesh.verts[i].color[3] = 255;

		baseMesh.verts[i].text.x = cubeTextCoords[i * 2];
		baseMesh.verts[i].text.y = cubeTextCoords[i * 2 + 1];

		baseMesh.verts[i].selected = 0.0;
	}

}

void initializeShaders()
{
	blockShader.initialize("Shaders/");
	gridShader.initialize("Shaders/");
}


//get the SDL user input
void control()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			gameState = EXIT;
			return;
		case SDL_MOUSEMOTION:
			if (MouseButtons[SDL_BUTTON_RIGHT]){
				mainCamera->mouseMove(evnt.motion.xrel, evnt.motion.yrel);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			MouseButtons[evnt.button.button] = 1;
			if (MouseButtons[SDL_BUTTON_LEFT]){
				glm::vec3 temp = mainCamera->screenToWorld(glm::vec2(evnt.motion.x, evnt.motion.y), graphicsOptions.screenWidth, graphicsOptions.screenHeight);
				printf("mouse xyz coords = <%f, %f, %f>\n", temp.x, temp.y, temp.z);
				mainCamera->findIntersect(temp);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			MouseButtons[evnt.button.button] = 0;
			break;
		case SDL_MOUSEWHEEL:
			mainCamera->mouseZoom(evnt.wheel.y);
			break;
		case SDL_KEYDOWN:
			Keys[evnt.key.keysym.sym].pr = 1;
			switch (evnt.key.keysym.sym){
			case SDLK_g:
				if (gameGrid->vTot > 0){
					cout << "Remove Start\n";
					for (int i = 0; i < gameGrid->w; i++){
						for (int j = 0; j < gameGrid->h; j++){
							for (int k = 0; k < gameGrid->l; k++){
								gameGrid->removeVoxel(i, j, k);
							}
						}
					}
					cout << "Remove end\n";
				}
				else{
					for (int i = 0; i < gameGrid->w; i++){
						for (int j = 0; j < gameGrid->h; j++){
							for (int k = 0; k < gameGrid->l; k++){
								gameGrid->addVoxel(currentVox, i, j, k);
							}
						}
					}
				}
				break;
			case SDLK_t:
				if (state == 's'){
					printf("Insert mode activated\n");
					state = 'i';
				}
				else if (state == 'i'){
					printf("Remove mode activated\n");
					state = 'r';
				}
				else {
					printf("Select mode activated\n");
					state = 's';
				}
				break;
			case SDLK_i:
				int addx, addy, addz;
				cout << "x: ";
				cin >> addx;
				cout << "y: ";
				cin >> addy;
				cout << "z: ";
				cin >> addz;
				gameGrid->addVoxel(currentVox, addx, addy, addz);
				break;
			case SDLK_r:
				int rx, ry, rz;
				cout << "x: ";
				cin >> rx;
				cout << "y: ";
				cin >> ry;
				cout << "z: ";
				cin >> rz;
				gameGrid->removeVoxel(rx, ry, rz);
				break;
			}
			break;

		case SDL_KEYUP:
			Keys[evnt.key.keysym.sym].pr = 0;
			break;
		}
	}
}

void update()
{
    voxelEditor.update();
	mainCamera->update();
}

//draws the scene
void draw()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    voxelEditor.draw(mainCamera);

    SDL_GL_SwapWindow(mainWindow);
}

//hulk smash!
void destroy()
{
	//this stuff is technically optional, since it should happen on app exit anyways
	SDL_GL_DeleteContext(mainOpenGLContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
}