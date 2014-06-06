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
#include "Texture.h"
#include "RenderUtil.h"

void initialize();
void initializeSdlOpengl();
void initializeVertexBuffer();
void initializeShaders();
void initializeTextures();
void control();
void update();
void draw();
void drawGrid();
void destroy();
bool checkGlError();

enum states{INITIALIZE, PLAY, EXIT};

int gameState;

SDL_Window *mainWindow;
SDL_GLContext mainOpenGLContext;

Camera *mainCamera;

map <char, texture*> cubeTexts;
map <char, texture*> cubeSelectedTexts;

vector <BlockVertex> currentVerts;
vector <int> currentIndices;
BlockMesh baseMesh;
voxel *currentVox;
bool changed = true;

//vector <texture*> cubeTexts;
//vector <texture*> cubeSelectedTexts;

grid *gameGrid;



int main(int argc, char **argv)
{
	initialize();

	while (gameState == PLAY){
		if (checkGlError()){
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

	gameGrid = new grid(10,10,10);
	currentVox = new voxel;
	currentVox->type = 'b';
	currentVox->selected = false;

	initializeSdlOpengl();
	initializeVertexBuffer();
	initializeShaders();
	initializeTextures();

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
	currentVerts.reserve(24 * cubeTot / 2);
	
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

		baseMesh.verts[i].offset.x = 0;
		baseMesh.verts[i].offset.y = 0;
		baseMesh.verts[i].offset.z = 0;
		/*baseMesh.verts[i * 24 + j].offset.x = (i % (w * l)) % l;
		baseMesh.verts[i * 24 + j].offset.y = (i % (w * l)) / l;
		baseMesh.verts[i * 24 + j].offset.z = i / (w * l);*/
	}
	gameGrid->addVoxel(currentVox, 0, 0, 0);
}

void initializeShaders()
{
	blockShader.initialize("Shaders/");
	gridShader.initialize("Shaders/");
}

void initializeTextures()
{
	cubeTexts.insert(make_pair('b', new texture("Data/black.png", 'b')));
	cubeSelectedTexts.insert(make_pair('b', new texture("Data/blackSelected.png", 'b')));
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
			break;
		case SDL_KEYUP:
			Keys[evnt.key.keysym.sym].pr = 0;
			break;
		}
	}
}

void update()
{
	mainCamera->update();
}

//draws the scene
void draw()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Opengl Resource for Opengl 3.3+ http://www.opengl-tutorial.org/

	//*************** here is some example draw code. This is temporary, and should not really be used. ****************
	blockShader.bind();

	glm::mat4 modelMatrix(1);
	//this is a fast way to set up the translation. This is equivalent to a translatef
	//We translate by the negative position of the camera. This causes the world to move around the camera, rather 
	//than the camera to move around the world.
	modelMatrix[3][0] = -mainCamera->position.x;
	modelMatrix[3][1] = -mainCamera->position.y;
	modelMatrix[3][2] = -mainCamera->position.z;

	//glm::mat4 MVP = mainCamera->projectionMatrix * mainCamera->viewMatrix * modelMatrix;
	glm::mat4 VP = mainCamera->projectionMatrix * mainCamera->viewMatrix;
	glm::mat4 M = modelMatrix;

	//send our uniform data, the matrix, the light position, and the texture data
	glUniformMatrix4fv(blockShader.mvpID, 1, GL_FALSE, &VP[0][0]);
	glUniformMatrix4fv(blockShader.mID, 1, GL_FALSE, &M[0][0]);
	glm::vec3 lightPos(0.1f, 0.5f, 0.8f);
	lightPos = glm::normalize(lightPos);
	glUniform3f(blockShader.lightPosID, lightPos.x, lightPos.y, lightPos.z);
	
	//t key toggles between selected/not selected texture
	//if (Keys[SDLK_t].pr == 0){
	if (gameGrid->getVoxel(0,0,0) != NULL){
		if (gameGrid->getVoxel(0,0,0)->selected == false){
			glBindTexture(GL_TEXTURE_2D, cubeTexts['b']->data);
			checkGlError();
			glUniform1i(blockShader.textPosID, 0);
		}
		else{
			glBindTexture(GL_TEXTURE_2D, cubeSelectedTexts['b']->data);
			checkGlError();
			glUniform1i(blockShader.textPosID, 0);
		}
	}
	else{
		glBindTexture(GL_TEXTURE_2D, cubeTexts['b']->data);
		checkGlError();
		glUniform1i(blockShader.textPosID, 0);
	}

	//glUniform1f(blockShader.selected, 1);

	//Static variables, so they are intitialized onces and remain for the life of the program.
	//In reality, your meshes should be stored in a class somewhere, but this is just an example
	static GLuint vboID = 0;
	static GLuint elementsID = 0;

	//initialize the buffer, only happens once
	//if (vboID == 0){
	if (changed == 1){
		//generate a buffer object for the vboID. after this call, vboID will be a number > 0
		glGenBuffers(1, &vboID);
		//generate buffer object for the indices
		glGenBuffers(1, &elementsID);


		//Generate the cube's vertex data
		//BlockVertex verts[24];
		//for (int i = 0; i < 24; i++){
		//	verts[i].position.x = cubeVertices[i * 3];
		//	verts[i].position.y = cubeVertices[i * 3 + 1];
		//	verts[i].position.z = cubeVertices[i * 3 + 2];

		//	verts[i].normal.x = cubeNormals[i * 3];
		//	verts[i].normal.y = cubeNormals[i * 3 + 1];
		//	verts[i].normal.z = cubeNormals[i * 3 + 2];

		//	verts[i].color[0] = 0;
		//	verts[i].color[1] = 255;
		//	verts[i].color[2] = 0;
		//	verts[i].color[3] = 255;

		//	verts[i].text.x = cubeTextCoords[i * 2];
		//	verts[i].text.y = cubeTextCoords[i * 2 + 1];
		//}
		//
		////the indexes for drawing the cube, it just follows the pattern (i, i+1, i+2, i+2, i+3, i) for i += 4
		//GLuint drawIndices[36] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };

		//bind the buffers into the correct slots
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		//this call is optional, but it makes it faster (not in this case) because it orphans any previous buffer. opengl-tutorial has details
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), NULL);
		//fill the buffer with our vertex data. This is basically a memcpy. Static draw means we change the buffer once and draw many times
		//glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, currentVerts.size(), &currentVerts[0], GL_STATIC_DRAW);

		//now do the same thing for the elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
		//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(drawIndices), NULL);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(drawIndices), drawIndices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentIndices.size(), &currentIndices[0], GL_STATIC_DRAW);
		changed = false;
	}
	else{ //we already initialized the buffers on another frame
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
	}

	//set our attribute pointers using our interleaved vertex data. Last parameter is offset into the vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)0); //vertexPosition
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(BlockVertex), (void *)12); //vertexColor
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)16); //vertexNormal
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)28); //textureCoordinates
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void *)36); //textureCoordinates

	//Finally, draw our data. The last parameter is the offset into the bound buffer
	glDrawElements(GL_TRIANGLES, currentIndices.size(), GL_UNSIGNED_INT, NULL);

	blockShader.unBind();

    if (drawDebugLine){
        RenderUtil::drawLine(mainCamera, debugP1, debugP2, 255, 0, 255, 5);
    }

	drawGrid();

	//SDL_GL_SwapWindow(mainWindow);
}

void drawGrid()
{
	//glClearDepth(1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Opengl Resource for Opengl 3.3+ http://www.opengl-tutorial.org/

	//*************** here is some example draw code. This is temporary, and should not really be used. ****************
	gridShader.bind();

	glm::mat4 modelMatrix(1);
	//this is a fast way to set up the translation. This is equivalent to a translatef
	//We translate by the negative position of the camera. This causes the world to move around the camera, rather 
	//than the camera to move around the world.
	modelMatrix[3][0] = -mainCamera->position.x;
	modelMatrix[3][1] = -mainCamera->position.y;
	modelMatrix[3][2] = -mainCamera->position.z;


	glm::mat4 MVP = mainCamera->projectionMatrix * mainCamera->viewMatrix * modelMatrix;

	//send our uniform data, the matrix, the light position, and the texture data
	glUniformMatrix4fv(gridShader.mvpID, 1, GL_FALSE, &MVP[0][0]);

	//Static variables, so they are intitialized onces and remain for the life of the program.
	//In reality, your meshes should be stored in a class somewhere, but this is just an example
	static GLuint vboID = 0;
    static int sizeHolder = 0;
	//initialize the buffer, only happens once
	if (vboID == 0){
		//generate a buffer object for the vboID. after this call, vboID will be a number > 0
		glGenBuffers(1, &vboID);

		//Generate the cube's vertex data

		GridVertex* verts;
		int i = 0, w, l;
		w = gameGrid->w;
		l = gameGrid->l;
		sizeHolder = ((w + 1) + (l + 1)) * 2;
		verts = new GridVertex[sizeHolder];
		int alpha = 255;

		for (int j = 0; j < w + 1; j++, i += 2){
			verts[i].position.x = w - j;
			verts[i].position.y = 0;
			//verts[i].position.z = -(l / 2);
			verts[i].position.z = 0;

			verts[i + 1].position.x = w - j;
			verts[i + 1].position.y = 0;
			verts[i + 1].position.z = l;

			verts[i].color[0] = 255;
			verts[i].color[1] = 0;
			verts[i].color[2] = 0;
			verts[i].color[3] = alpha;

			verts[i + 1].color[0] = 255;
			verts[i + 1].color[1] = 0;
			verts[i + 1].color[2] = 0;
			verts[i + 1].color[3] = alpha;
		}

		for (int k = 0; k < l + 1; k++, i += 2){
			verts[i].position.x = 0;
			verts[i].position.y = 0;
			verts[i].position.z = l - k;

			verts[i + 1].position.x = w;
			verts[i + 1].position.y = 0;
			verts[i + 1].position.z = l - k;

			verts[i].color[0] = 255;
			verts[i].color[1] = 0;
			verts[i].color[2] = 0;
			verts[i].color[3] = alpha;

			verts[i + 1].color[0] = 255;
			verts[i + 1].color[1] = 0;
			verts[i + 1].color[2] = 0;
			verts[i + 1].color[3] = alpha;
		}

		//bind the buffers into the correct slots
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		//this call is optional, but it makes it faster (not in this case) because it orphans any previous buffer. opengl-tutorial has details
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), NULL);
		//fill the buffer with our vertex data. This is basically a memcpy. Static draw means we change the buffer once and draw many times
		glBufferData(GL_ARRAY_BUFFER, sizeHolder * sizeof(GridVertex), verts, GL_STATIC_DRAW);
	}
	else{ //we already initialized the buffers on another frame
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//set our attribute pointers using our interleaved vertex data. Last parameter is offset into the vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void *)0); //vertexPosition
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GridVertex), (void *)12); //vertexColor


	//Finally, draw our data. The last parameter is the offset into the bound buffer
	glLineWidth(1);
	glDrawArrays(GL_LINES, 0, sizeHolder);

	gridShader.unBind();

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


bool checkGlError(){
	GLenum err = glGetError();
	if (err != GL_NO_ERROR){
		switch (err){
		case GL_OUT_OF_MEMORY:
			error("Out of memory! Try lowering the voxel view distance.");
			return 1;
		case GL_INVALID_ENUM:
			error("GL_INVALID_ENUM - An unacceptable value is specified for an enumerated argument.");
			return 0;
		case GL_INVALID_VALUE:
			error("GL_INVALID_VALUE - A numeric argument is out of range.");
			return 0;
		case GL_INVALID_OPERATION:
			error("GL_INVALID_OPERATION - The specified operation is not allowed in the current state.");
			return 0;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error("The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete.");
			return 0;
		default:
			error(("OpenGL ERROR (" + to_string(err) + string(") ")).c_str());
			return 0;
		}
	}
	return 0;
}