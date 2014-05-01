#pragma once
//
// VoxelEditor.h
//
// Copyright 2014 Seed Of Andromeda
//
#include <GL/glew.h>
#include <string>

using namespace std;

//describes a voxel
struct VoxelType
{
	string name;
	GLubyte color[4];
};


class VoxelEditor
{
public:
	VoxelEditor();
	~VoxelEditor();

	/*TODO: Everything
	void initialize();

	void drawGrid();
	void drawVoxels();

	void update();

	void placeVoxels();
	void removeVoxels();

private:
	//data is an array of indices, that index into voxelTypes to see which voxel is at a certain point
	GLushort *data; //dynamic array for resizable grid?
	int width, height;

	*/
};

