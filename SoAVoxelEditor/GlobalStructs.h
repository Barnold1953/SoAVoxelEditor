#pragma once
//
// GlobalStructs.h
//
// Copyright 2014 Seed Of Andromeda
//
// put global data and structs here
#include <Windows.h>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <map>

using namespace std;

//this is for input
//I encase a boolean in a struct so that the key is guarenteed to be initialized to false
struct SOAKEY{
	SOAKEY() : pr(0) {}
	bool pr; //pressed
};
extern map <GLuint, SOAKEY> Keys;
extern bool MouseButtons[10];

//interleaved vertex data for a BlockShading
struct BlockVertex{
	glm::vec3 position;
	GLubyte color[4];
	glm::vec3 normal;
};


//here are the vertex positions for the 24 cube vertices
static GLfloat cubeVertices[72] = { 0.0f, 1.000f, 1.000f, 0.0f, 0.0f, 1.000f, 1.000f, 0.0f, 1.000f, 1.000f, 1.000f, 1.000f,  // v1-v2-v3-v0 (front)

1.000f, 1.000f, 1.000f, 1.000f, 0.0f, 1.000f, 1.000f, 0.0f, 0.0f, 1.000f, 1.000f, 0.0f,     // v0-v3-v4-v5 (right)

0.0f, 1.000f, 0.0f, 0.0f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 0.0f,    // v6-v1-v0-v5 (top)

0.0f, 1.000f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.000f, 0.0f, 1.000f, 1.000f,   // v6-v7-v2-v1 (left)

0.0f, 0.0f, 0.0f, 1.000f, 0.0f, 0.0f, 1.000f, 0.0f, 1.000f, 0.0f, 0.0f, 1.000f,    // v7-v4-v3-v2 (bottom)

1.000f, 1.000f, 0.0f, 1.000f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.000f, 0.0f };     // v5-v4-v7-v6 (back)

//heres the normals
static float cubeNormals[72] = { 0, 0, 1.0f, 0, 0, 1.0f, 0, 0, 1.0f, 0, 0, 1.0f,  // v1-v2-v3-v0 (front)

1.0f, 0, 0, 1.0f, 0, 0, 1.0f, 0, 0, 1.0f, 0, 0,     // v0-v3-v4-v5 (right)

0, 1.0f, 0, 0, 1.0f, 0, 0, 1.0f, 0, 0, 1.0f, 0,    // v6-v1-v0-v5 (top)

-1.0f, 0, 0, -1.0f, 0, 0, -1.0f, 0, 0, -1.0f, 0, 0,   // v6-v7-v2-v1 (left)

0, -1.0f, 0, 0, -1.0f, 0, 0, -1.0f, 0, 0, -1.0f, 0,    // v7-v4-v3-v2 (bottom)

0, 0, -1.0f, 0, 0, -1.0f, 0, 0, -1.0f, 0, 0, -1.0f };     // v5-v4-v7-v6 (back)