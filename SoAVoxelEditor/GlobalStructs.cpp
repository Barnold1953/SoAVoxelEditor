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

grid::grid(int height, int width, int length){
	voxels.resize(height*width*length);
	for (int i = 0; i < height*width*length; i++){
		voxels[i] = new voxel;
		voxels[i]->type = '\0';
		voxels[i]->selected = 0;
	}
	h = height;
	w = width;
	l = length;
	layerSize = w * l;
}

//i*layersize + j*width + k
//Y, Z, X basically‏

void grid::addVoxel(voxel* newV, int x, int y, int z){
	/*x += (w/2) - 1;
	y += (h/2) - 1;
	z += (l/2) - 1;*/
	voxel *tempV = getVoxel(x, y, z);
	if (tempV->type == '\0'){
		tempV = newV;
	}
	else{
		printf("Voxel space <%d,%d,%d> is occupied.\n", x, y, z);
	}
}
void grid::removeVoxel(int x, int y, int z){
	/*x += (w / 2) - 1;
	y += (h / 2) - 1;
	z += (l / 2) - 1;*/

	voxel *tempV = getVoxel(x, y, z);
	if (tempV->type == '\0'){
		printf("Nothing to remove at <%d,%d,%d>.\n", x, y, z);
	}
	else{
		tempV->type = '\0';
		tempV->selected = 0;
	}

}
voxel* grid::getVoxel(int x, int y, int z){
	x += (w / 2);
	y += (h / 2);
	z += l;

	//printf("getVoxel coordinates <%d,%d,%d>\n", x, y, z);
	bool rangeCheck = false;

	if (x < 0 || x >= w){
		printf("Value: %d for x is out of range\n", x);
		rangeCheck = true;
	}
	if (y < 0 || y >= h){
		printf("Value: %d for y is out of range\n", y);
		rangeCheck = true;
	}
	if (z < 0 || z >= l){
		printf("Value: %d for z is out of range\n", z);
		rangeCheck = true;
	}
	if (rangeCheck == true){
		return NULL;
	}

	return voxels[y*layerSize + z*l + x];
}