#pragma once
//
// GlobalStructs.cpp
//
// Copyright 2014 Seed Of Andromeda
//
// put global data and structs here
#include "GlobalStructs.h"

glm::vec3 debugP1, debugP2;
bool drawDebugLine;

map <GLuint, SOAKEY> Keys;
bool MouseButtons[10];

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
	vTot = 0;
}

//i*layersize + j*width + k
//Y, Z, X basically‏

void grid::addVoxel(voxel* newV, int x, int y, int z){
	bool rangeCheck = false;

	if (x < 0 || x >= w){
		//printf("Value: %d for x is out of range\n", x);
		rangeCheck = true;
	}
	if (y < 0 || y >= l){
		//printf("Value: %d for y is out of range\n", y);
		rangeCheck = true;
	}
	if (z < 0 || z >= h){
		//printf("Value: %d for z is out of range\n", z);
		rangeCheck = true;
	}
	if (rangeCheck == true){
		return;
	}

	voxel *tempV = getVoxel(x, y, z);
	if (tempV->type == '\0'){
		tempV->type = newV->type;
		tempV->selected = newV->selected;
		vTot++;
		for (int i = 0; i < 24; i++){
			baseMesh.verts[i].offset.x = x;
			baseMesh.verts[i].offset.y = y;
			baseMesh.verts[i].offset.z = z;
			currentVerts.push_back(baseMesh.verts[i]);
		}
		for (int i = currentVerts.size(); i < currentVerts.size() + 24;  i += 4){
			currentIndices.push_back(i);
			currentIndices.push_back(i+1);
			currentIndices.push_back(i+2);
			currentIndices.push_back(i+2);
			currentIndices.push_back(i+3);
			currentIndices.push_back(i);
		}
		changed = true;
	}
	else{
		printf("Voxel space <%d,%d,%d> is occupied.\n", x, y, z);
	}
	printf("Voxel at <%d,%d,%d> added.\n", x, y, z);
}
void grid::removeVoxel(int x, int y, int z){
	bool rangeCheck = false;

	if (x < 0 || x >= w){
		//printf("Value: %d for x is out of range\n", x);
		rangeCheck = true;
	}
	if (y < 0 || y >= l){
		//printf("Value: %d for y is out of range\n", y);
		rangeCheck = true;
	}
	if (z < 0 || z >= h){
		//printf("Value: %d for z is out of range\n", z);
		rangeCheck = true;
	}
	if (rangeCheck == true){
		return;
	}

	voxel *tempV = getVoxel(x, y, z);
	if (tempV->type == '\0'){
		printf("Nothing to remove at <%d,%d,%d>.\n", x, y, z);
	}
	else{
		tempV->type = '\0';
		tempV->selected = 0;
		vTot--;
		for (int i = 0; i < currentVerts.size(); i++){
			if (currentVerts[i].offset.x == x && currentVerts[i].offset.y == y && currentVerts[i].offset.z == z){
				currentVerts.erase(currentVerts.begin() + i, currentVerts.begin() + i + 24);
				currentIndices.erase(currentIndices.begin() + i, currentIndices.begin() + i + 24);
			}
		}
		changed = true;
	}
	printf("Voxel at <%d,%d,%d> removed.\n", x, y, z);
}
voxel* grid::getVoxel(int x, int y, int z){
	//printf("getVoxel coordinates <%d,%d,%d>\n", x, y, z);
	bool rangeCheck = false;

	if (x < 0 || x >= w){
		//printf("Value: %d for x is out of range\n", x);
		rangeCheck = true;
	}
	if (y < 0 || y >= l){
		//printf("Value: %d for y is out of range\n", y);
		rangeCheck = true;
	}
	if (z < 0 || z >= h){
		//printf("Value: %d for z is out of range\n", z);
		rangeCheck = true;
	}
	if (rangeCheck == true){
		return NULL;
	}

	return voxels[z*layerSize + y*w + x];
}