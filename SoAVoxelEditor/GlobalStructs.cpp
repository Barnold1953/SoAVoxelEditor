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

grid::grid(int width, int height, int length){
	voxels.resize(height*width*length);
	for (int i = 0; i < height*width*length; i++){
		voxels[i] = new voxel;
		voxels[i]->type = '\0';
		voxels[i]->selected = 0;
	}
	h = height;
	w = width;
	l = length;
	layerSize = w * h;
	vTot = 0;
}

//i*layersize + j*width + k
//Y, Z, X basically‏

void grid::addVoxel(voxel* newV, int x, int y, int z){
	voxel *tempV = getVoxel(x, y, z);
	BlockVertex tv;
	if (tempV == NULL){
		return;
	}
	if (tempV->type == '\0'){
		tempV->type = newV->type;
		tempV->selected = newV->selected;
		vTot++;

		for (int i = 0; i < 24; i++){
			tv = baseMesh.verts[i];
			tv.position.x += x;
			tv.position.y += y;
			tv.position.z += z;
			tv.selected = 0.0;
			currentVerts.push_back(tv);
		}
		changed = true;
		//printf("Voxel at <%d,%d,%d> added.\n", x, y, z);
	}
	else{
		printf("Voxel space <%d,%d,%d> is occupied.\n", x, y, z);
	}
}
void grid::removeVoxel(int x, int y, int z){
	voxel *tempV = getVoxel(x, y, z);
	if (tempV == NULL){
		return;
	}
	if (tempV->type == '\0'){
		printf("Nothing to remove at <%d,%d,%d>.\n", x, y, z);
	}
	else{
		tempV->type = '\0';
		tempV->selected = 0;
		vTot--;
		for (int i = 0; i < currentVerts.size(); i++){
			if (currentVerts[i].position.x - baseMesh.verts[i % 24].position.x == x && currentVerts[i].position.y - baseMesh.verts[i % 24].position.y == y && currentVerts[i].position.z - baseMesh.verts[i % 24].position.z == z){
			//if (currentVerts[i].offset.x == x && currentVerts[i].offset.y == y && currentVerts[i].offset.z == z){
				for (int j = 0; j < 24; j++){
					currentVerts[i+j] = currentVerts[currentVerts.size() - 24 + j];
				}
				for (int j = 0; j < 24; j++){
					currentVerts.pop_back();
				}
				//currentVerts.erase(currentVerts.begin() + i, currentVerts.begin() + i + 24);
				break;
			}
		}
		changed = true;
		//printf("Voxel at <%d,%d,%d> removed.\n", x, y, z);
	}
}
voxel* grid::getVoxel(int x, int y, int z){
	//printf("getVoxel coordinates <%d,%d,%d>\n", x, y, z);
	bool rangeCheck = false;

	if (x < 0 || x >= w){
		//printf("Value: %d for x is out of range\n", x);
		rangeCheck = true;
	}
	if (y < 0 || y >= h){
		//printf("Value: %d for y is out of range\n", y);
		rangeCheck = true;
	}
	if (z < 0 || z >= l){
		//printf("Value: %d for z is out of range\n", z);
		rangeCheck = true;
	}
	if (rangeCheck == true){
		return NULL;
	}

	return voxels[z*layerSize + y*w + x];
}