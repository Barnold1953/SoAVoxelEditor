#pragma once
//
// GlobalStructs.cpp
//
// Copyright 2014 Seed Of Andromeda
//
// put global data and structs here
#include "GlobalStructs.h"

map <GLuint, SOAKEY> Keys;
bool MouseButtons[10];

glm::vec2 get2dPoint(glm::vec3 location, glm::mat4 view, glm::mat4 projection, int height, int width){
	glm::mat4 viewProd = projection * view;
	glm::vec2 newCoord;
	glm::vec4 container = (glm::vec4(location, 0.0f) * viewProd);
	location.x = container.x;
	location.y = container.y;
	location.z = container.z;

	newCoord.x = (int)round(((location.x + 1) / 2.0)*width);
	newCoord.y = (int)round(((1 - location.y) / 2.0)*height);

	return newCoord;
}
glm::vec3 get3dPoint(glm::vec2 location, glm::mat4 view, glm::mat4 projection, int height, int width){
	double x = 2.0 * location.x / width - 1;
	printf("%lf\n", x);
	double y = 2.0 * location.y / height - 1;
	glm::mat4 viewProd = glm::inverse(projection * view);
	glm::vec4 container = glm::vec4(x, y, 0.0f, 0.0f) * viewProd;

	float z = 0.0f;
	//glReadBuffer(GL_FRONT);
	//glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &z);
	return (glm::vec3(container.x, container.y, z));
}