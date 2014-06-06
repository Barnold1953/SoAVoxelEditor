#pragma once
//
// Shader.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <Windows.h>
#include <GL/glew.h>
#include <string>
#include <SOIL.h>

using namespace std;

class texture{
public:
	texture(string location, char tag);
	GLuint data;
	char type;
};