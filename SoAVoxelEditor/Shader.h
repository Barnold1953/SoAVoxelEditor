#pragma once
//
// Shader.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <Windows.h>
#include <GL/glew.h>
#include <string>

using namespace std;

GLuint GetUniform(GLuint shader, const char * name);

//shader class
class Shader
{
public:
	Shader() : initialized(0){}
	GLuint shaderID;

	void initialize(){};
	void deleteShader();
	void bind(){};
	void unBind(){};
	bool isInitialized(){ return initialized; }

	GLuint mvpID;
	GLuint lightPosID;
protected:
	bool initialized;
};

//block rendering class
class BlockShader : public Shader
{
public:
	void initialize(string dirPath);
	void bind();
	void unBind();

    GLuint colorID;
};

class GridShader : public Shader
{
public:
	void initialize(string dirPath);
	void bind();
	void unBind();
};

class WireframeShader: public Shader {
public:
    void initialize(string dirPath);
    void bind();
    void unBind();

    GLuint colorID;
};

extern BlockShader blockShader;
extern GridShader gridShader;
extern WireframeShader wireframeShader;