#pragma once
//
// VoxelEditor.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <GL/glew.h>
#include <string>

#include "VoxelGrid.h"

using namespace std;

class VoxelEditor
{
public:
	VoxelEditor();
	~VoxelEditor();

	void initialize();

	void draw(class Camera *camera);

	void update();

	void placeVoxels();
	void removeVoxels();

private:
	VoxelGrid *voxelGrid; 
    voxel *currentVoxel;
	int width, height;
};

